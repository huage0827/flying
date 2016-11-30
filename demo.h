#ifndef DEMO_H_INCLUDED
#define DEMO_H_INCLUDED
#include "core.h"
namespace fs{

    class FlyingSpore;
    class SporeBuilder;
    class FormatBuilder;

    class CellContext;
    class DataPackage;
    //数值定位符号，类似于: fileInfo/state这样的路径
    class ValueMark;
    class Nest;

}

namespace myspore{


}


int __main(){


    auto nest = fs::LocalNest();

    //定义一个 spore builder，指定基类
    auto b_root = fs::SporeBuilder<fs::FlyingSpore>(nest);


    //为root spore 添加环境变量
    struct AVCode;
    struct av_context{
        AVCode code;
    };
    struct AVFrame;
    /*	
    定义数据格式
    数据格式（format）本身有名字、属性，并且是可序列化的；
    内部有任意条条目（entry），条目有名字、类型、属性（0-*）
    标准类型的条目通过名称访问(底层可能的话需要映射成ID来访问这样快些)
    自定义类型的话，在传递过程中可能类型会丢失，在使用时需做显示类型转，不是类型安全的操作方式
    */
    auto df_base = fs::make_fromat("{filename:string; duration:unsigned int; postion: unsigned int; size{width:int; height:int;}; state:int}");
    auto df_av = fs::make_fromat<av_context>();
    auto df_avFrame = fs::make_fromat<AVFrame>();
    auto df_cmd = fs::make_fromat("{cmdCode:int;seekPos:int(*); }");
    //也可以注册到格式定义系统
    nest.reg_format("df_my_spore_evn", df_base);

    /*
    通过add_context添加环境变量，返回的应该是share_prt<>，使用环境变量的好处是它可以被系列化，传递或者同步到另一个cell
    */
    //字符串格式的环境变量,这些环境变量可以被父spore引用
    auto ctx_root_base = b_root.add_context("fileInfo", "{filename:string; duration:unsigned int; postion: unsigned int; size : {width:int; height:int;}; state:int}");
    //或者
    ctx_root_base = b_root.add_context("fileInfo", df_base);
    //或者
    ctx_root_base = b_root.add_context("fileInfo", fs::local_format("df_my_spore_evn"));

    //自定义类型环境变量
    auto ctx_root_av = b_root.add_context<av_context>("AVInfo");
    //或者
    ctx_root_av = b_root.add_context("AVInfo", df_av);

    /*
    分配执行器， 它从父cell（spore）的池里拆分出需要的执行器
    */
    auto act_dec = b_root.alloc_actuator(1, "act_dec");

    //添加输出pin，负责输出解包后的Pack
    auto pin_out_avpack = b_root.add_out_pin("out_avpack", 
        df_avFrame
        );

    //添加输出pin，负责输出“继续解包”的控制命令
    auto pin_out_next = b_root.add_out_pin("out_next", 
        df_cmd
        );

    //添加一个输出Pin，内部的环境变量变化时输出
    /*
    在指定数据格式时，可以使用“=”来绑定到环境变量，使用“@”来指明触发条件，如：
    "{state:int = %env%.state@after; msg:string;}"
    "{%env%.state@after; msg:string;}"
    */
    b_root.add_out_pin("out_env_changed",
        "{state:int = %env%.state@after; msg:string;}",//当%env%.state发生“变化后”会执行后续的funtion
        [](const CellContext &context, const ValueMark &mark, const DataPackage &pack)
    {
        pack["msg"] = "the state is changed";
    }
    );

    //添加输入Pin，同时定义格式、执行函数、默认执行器
    //这个Pin负责接收文件名称
    auto pin_in_file_name = b_root.add_in_pin("in_file_name",
        "{filename:string;}",
        [&](const CellContext &context, const DataPackage &pack)
    {
        //初始化av解码环境，这里可以设置环境变量

        //可以通过context变量来访问环境
        context["fileInfo"]["filename"] = pack["filename"].asString();
        context["fileInfo"]["duration"] = 1000;
        //也可以通过显示的引用来访问环境
        ctx_root_av.code = NULL;
        //或者显示转换为自定义的类型
        auto ctx_av = context.as<av_context>("AVInfo");
        /*
        上面的"[]"访问方式有可能返回的是不存在的环境变量，这时，context会构建一个空对象来赋值，或者抛出异常。
        */
        ctx_av.av = NULL;
    }
    );

    //添加输入Pin，用于控制解包, 使用独立的执行器
    auto pin_in_control = b_root.add_in_pin("in_control",
        df_cmd,
        [&](const CellContext &context, const DataPackage &pack)
    {
        //这里通过命令码控制解码器
        switch(pack["cmdCode"].asInt())
        {
        case 0: //开始解包
            {
                //获取流里的Frame
                //...
                //...
                //通过输出Pin构造一个packbuilder
                auto b_pack =  pin_out_avpack.pack_builder();
                //填充builder
                //...
                //...
                //输出包
                pin_out_avpack.add_pack(b_pack);

                /*
                判断当前的state看看是否需要停止，否则构造一个cmdCode=0的控制包给自己，目的是构造一个自循环。有两种方式：
                1.直接通过自己的Pin（pin_in_file_name）或者context（通过名字取出Pin）推送这个包到队列里；
                2.再构造一个输出Pin，输出到这个pin，然后由外部链接输出Pin到自己的Pin；
                第1种方式相当于隐藏了自循环的逻辑，它的启动由第一次pack的输入来启动，然后自己决定是否循环，
                第2种方式将循环控制的权利交由外部，这样外部可以挂机其他的Spore来扩控制

                */
                //第一种方式
                auto b_next_cmd =  pin_out_next.pack_builder();
                b_next_cmd["cmdCode"] = 0;
                pin_out_next.add_pack(b_next_cmd);
                //第二种方式
                auto b_next_cmd0 =  pin_in_control.pack_builder();
                b_next_cmd0["cmdCode"] = 0;
                pin_in_control.add_pack(b_next_cmd0);
                //或者
                auto _pin_out_next = context.find(fs::_meta_pin_type::_LEFT, "in_control");
                if(_pin_out_next){
                    _pin_out_next.add_pack(b_next_cmd0);
                }
            }
        case 1:break; //停止
        case 2:break; //继续
        case 3:break;
        }
        //更新环境状态
        context["state"] = pack["cmdCode"].asInt();
    },
        act_dec
        );

    /*下面是解包和渲染的spore定义，它俩单独放在一个spore里，是因为解包后有可能有多个流，而每个流我们都需要两个单独的执行器来解码和渲染，
    这样我们用一个spore来完成一个流的解包和渲染，当有多个流时，动态的创建这个spore的cell
    */
    typedef fs::SporeBuilder<fs::FlyingSpore> OrgSpore;
    class AVStreamSpore : public OrgSpore{
    private:

    public:
        AVStreamSpore(const Nest &nest):OrgSpore(nest){

        }
    }


    //注册spore
    nest.reg_spore("RootSpore", b_root);
}

#endif // DEMO_H_INCLUDED