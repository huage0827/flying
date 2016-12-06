/*!
 * \file demo1.h
 * \date 2016/12/06 18:05
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: prototyping of flying spore 
 *
 * \note
*/

#ifndef DEMO_H_INCLUDED
#define DEMO_H_INCLUDED
#include "core.h"
#include "element.h"

using namespace fs;

//为root spore 添加环境变量
struct AVCode;
struct av_context{
    AVCode code;
};
struct AVFrame;

int __main(){


    //关于运行环境的配置


    cluster _cluster;
    //作为一个开放的Nest，最多32个执行器，具备全功能执行能力，可被发现、可被查询、可中转查询、可接受任务、可分派任务、可接受主控权、可转移主控权、可隐身访问
    Runtime rt(32, cluster::ability_t::sages);
    _cluster.publish(rt, "127.0.0.1", 6101/*链路端口*/, 6201/*业务端口*/ );
    //或者作为一个集群的受控Nest，接收集群配置的spore定义文件内的任务
    Runtime rt0;//可支持的运行时环境，默认为空，即是隐身的Nest，不能和其他的Nest链接，可访问该配置文件定义的环境内容
    _cluster.join(rt0, "https://github.com/kicsy/flying/demo/publish/process/Video_processing.fspd");
    //或者作为一个扩展的Nest，加入到另一个Nest的活动中
    _cluster.join(rt, "127.0.0.1"/*另一Nest的地址*/, 6102/*链路端口*/, 6202/*业务端口*/ );

    //定义一个 spore builder，指定基类
    auto b_root = spore_builder();

    /*	
    定义数据格式
    数据格式（format）本身有名字、属性，并且是可序列化的；
    内部有任意条条目（entry），条目有名字、类型、属性（0-*）
    标准类型的条目通过名称访问(底层可能的话需要映射成ID来访问这样快些)
    自定义类型的话，在传递过程中可能类型会丢失，在使用时需做显示类型转，不是类型安全的操作方式
    */
    data_format df_base = data_format_builder("df_my_spore_evn", "{filename:string; duration:unsigned int; postion: unsigned int; size{width:int; height:int;}; state:int}").to_data_format();
    data_format df_av = data_format_builder<av_context>("df_av_info").to_data_format();
    data_format df_avFrame = data_format_builder<AVFrame>().to_data_format();
    data_format df_cmd = data_format_builder("{cmdCode:int;seekPos:int(*); }").to_data_format();
    //注册到格式定义系统，不是必须的
    b_root.reg(df_base);

    /*
    通过add添加环境变量，使用环境变量的好处是它可以被系列化，传递或者同步到另一个cell
    */
    b_root.add("fileInfo", df_base);
    //自定义类型环境变量
    b_root.add("AVInfo", df_av);

    axon_builder _b_axon;
    //输出axon，负责输出解包后的Pack
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::OUT);
    _b_axon.set_data_format(df_avFrame);
    axon _axon_out_avpack = b_root.add("out_avpack", _b_axon.to_axon());

    //输出axon，负责输出“继续解包”的控制命令
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::OUT);
    _b_axon.set_data_format(df_cmd);
    axon _axon_out_next = b_root.add("out_next", _b_axon.to_axon());

    //添加一个输出axon，内部的环境变量变化时输出
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::OUT);
    _b_axon.set_data_format(data_format_builder("{state:int; msg:string;}").to_data_format());
    //设置axon的处理机制，需要对这个处理进行描述
    _b_axon.add_trigger(trigger::trigger_type::fixed,
        trigger(b_root.context(), data_format_path("fileInfo/state"), data_pack::signal::after_changed),
         //设置axon输出时的处理动作
        [](data_context &context, const axon &_axon, const data_pack &_in_pack){
            //这里，_in_pack其实和context指向相同的内容
            data_pack_builder _b_data_pack = _axon.get_data_pack_builder();
            _b_data_pack["state"] = context["fileInfo/state"];
            _b_data_pack["msg"] = "the state is changed";
            _axon.push(_b_data_pack.to_data_pack());
        }
    );
    b_root.add("out_env_changed", _b_axon.to_axon());


    //添加输入axon
    //这个axon负责接收文件名称
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::IN);
    _b_axon.set_data_format(data_format_builder("{filename:string;}").to_data_format());
    _b_axon.add_trigger(trigger::trigger_type::fixed,
        axon::signal::data_in,
        [](data_context &context, const axon &_axon, const data_pack &_in_pack){
            //这里_in_pack是从当前的axon来数据（由axon::signal::data_in信号触发）
            //初始化av解码环境，这里可以设置环境变量

            //可以通过context变量来访问环境
            context["fileInfo/filename"] = pack["filename"].asString();
            context["fileInfo/duration"] = 1000;
            //或者显示转换为自定义的类型
            auto ctx_av = context.as<av_context>("AVInfo");
            /*
            上面的"[]"访问方式有可能返回的是不存在的环境变量，这时，context会构建一个空对象来赋值，或者抛出异常。
            */
            ctx_av.av = NULL;
        }
    );
    b_root.add("in_file", _b_axon.to_axon());

    //添加输入axon，用于控制解包, 使用独立的执行器
    
    /*
    分配执行器， 它从父cell（spore）的池里拆分出需要的执行器
    */
    actuator act_demux(1, "act_demux");

    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::IN);
    _b_axon.set_data_format(df_cmd);
    _b_axon.set_actuator(act_demux);
    axon _axon = b_root.add("in_control", _b_axon.to_axon());

    //这里将这个输入axon的执行体挂在spore上
    b_root.add_trigger(trigger::trigger_type::fixed,
        trigger(_axon, axon::signal::data_in),
        [&](data_context &context, const axon &_axon, const data_pack &_in_pack){
            //这里通过命令码控制解码器
            switch(_in_pack["cmdCode"].asInt())
            {
            case 0: //开始解包
                {
                    //获取流里的Frame
                    //...
                    //...
                    //通过输出axon构造一个packbuilder
                    auto b_pack =  _axon_out_avpack.get_data_pack_builder();
                    //填充builder
                    //...
                    //...
                    //输出包
                    _axon_out_avpack.push(b_pack);

                    /*
                    判断当前的state看看是否需要停止，否则构造一个cmdCode=0的控制包给自己，目的是构造一个自循环。有两种方式：
                    1.直接通过自己的Pin（pin_in_file_name）或者context（通过名字取出Pin）推送这个包到队列里；
                    2.再构造一个输出Pin，输出到这个pin，然后由外部链接输出Pin到自己的Pin；
                    第1种方式相当于隐藏了自循环的逻辑，它的启动由第一次pack的输入来启动，然后自己决定是否循环，
                    第2种方式将循环控制的权利交由外部，这样外部可以挂机其他的Spore来扩控制

                    */
                    //第一种方式
                    auto b_next_cmd0 =  _axon.get_data_pack_builder();
                    b_next_cmd0["cmdCode"] = 0;
                    _axon.push(b_next_cmd0);
                    //第二种方式
                    auto b_next_cmd =  _axon_out_next.get_data_pack_builder();
                    b_next_cmd["cmdCode"] = 0;
                    _axon_out_next.push(b_next_cmd);
                    }
                }
            case 1:break; //停止
            case 2:break; //继续
            case 3:break;
            }
            //更新环境状态
            context["state"] = _in_pack["cmdCode"];
        }
    );

    //注册spore
    _cluster.reg("RootSpore", b_root.to_spore());
    /*
    至此，我们通过spore_builder构造了一个spore，这个spore是通过“装配”而来的，它由多个外部赋予的零配件组装而成。你会发现，它可以作为一个严格意义上的“类”，
    它被封装到一个不可修改的spore实体，它的“数据”使用context来承载，它的外部接口是每个axon，它的操作就是每个axon上的trigger的执行体，它的axon可以被标示
    为“可替换”，由于所有spore实体只有一个类型（就是spore），它天生就是多态。
    唯一和纯粹类不一样的是它没有类成员变量，这样内部数据操作都依赖于context，而context默认都是原子操作，开销比较大。
    解决办法是直接继承spore实现新的spore类，但仍然通过spore_builder来构造结构。
    */

    /*
    下面是解包和渲染的spore定义，它俩单独放在一个spore里，是因为解包后有可能有多个流，而每个流我们都需要两个单独的执行器来解码和渲染，
    这样我们用一个spore来完成一个流的解包和渲染，当有多个流时，动态的创建这个spore的cell
    */
    class stream_spore : public spore{
    private:

    public:
        stream_spore(){
			//添加环境变量
			add("AVInfo", df_av);

			actuator act_decode(1, "act_decode");
			actuator act_render(1, "act_render");

        }
    }

	//从builder生成spore
	spore _root = b_root.to_spore();
	stream_spore _stream_spore;
	_stream_spore.context["AVInfo"].sync_by(_root.context["AVInfo"]);

    //执行
	_cluster.run({_root, _stream_spore});
}

#endif // DEMO_H_INCLUDED