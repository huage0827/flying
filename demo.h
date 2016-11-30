#ifndef DEMO_H_INCLUDED
#define DEMO_H_INCLUDED
#include "core.h"
namespace fs{

    class FlyingSpore;
    class SporeBuilder;
    class FormatBuilder;
    class Pin;
    class Runtime;
    class CellContext;
    class DataPackage;
    //��ֵ��λ���ţ�������: fileInfo/state������·��
    class ValueMark;
    class Nest;

}

namespace myspore{


}


int __main(){


    //�������л���������


    Nest nest;
    //��Ϊһ�����ŵ�Nest�����32��ִ�������߱�ȫ����ִ���������ɱ����֡��ɱ���ѯ������ת��ѯ���ɽ������񡢿ɷ������񡢿ɽ�������Ȩ����ת������Ȩ�����������
    Runtime rt(32, AS_FULL_FUNCTION);
    nest.publish(rt, "127.0.0.1", 6101/*��·�˿�*/, 6201/*ҵ��˿�*/ );
    //������Ϊһ����Ⱥ���ܿ�Nest�����ռ�Ⱥ���õ�spore�����ļ��ڵ�����
    Runtime rt0;//��֧�ֵ�����ʱ������Ĭ��Ϊ�գ����������Nest�����ܺ�������Nest���ӣ��ɷ��ʸ������ļ�����Ļ�������
    nest.join(rt0, "https://github.com/kicsy/flying/demo/publish/process/Video_processing.fspd");
    //������Ϊһ����չ��Nest�����뵽��һ��Nest�Ļ��
    nest.join(rt, "127.0.0.1"/*��һNest�ĵ�ַ*/, 6102/*��·�˿�*/, 6202/*ҵ��˿�*/ );

    //����һ�� spore builder��ָ������
    auto b_root = fs::SporeBuilder<fs::FlyingSpore>();


    //Ϊroot spore ��ӻ�������
    struct AVCode;
    struct av_context{
        AVCode code;
    };
    struct AVFrame;
    /*	
    �������ݸ�ʽ
    ���ݸ�ʽ��format�����������֡����ԣ������ǿ����л��ģ�
    �ڲ�����������Ŀ��entry������Ŀ�����֡����͡����ԣ�0-*��
    ��׼���͵���Ŀͨ�����Ʒ���(�ײ���ܵĻ���Ҫӳ���ID������������Щ)
    �Զ������͵Ļ����ڴ��ݹ����п������ͻᶪʧ����ʹ��ʱ������ʾ����ת���������Ͱ�ȫ�Ĳ�����ʽ
    */
    auto df_base = b_root.make_fromat("{filename:string; duration:unsigned int; postion: unsigned int; size{width:int; height:int;}; state:int}");
    auto df_av = b_root.make_fromat<av_context>();
    auto df_avFrame = b_root.make_fromat<AVFrame>();
    auto df_cmd = b_root.make_fromat("{cmdCode:int;seekPos:int(*); }");
    //Ҳ����ע�ᵽ��ʽ����ϵͳ
    b_root.reg_format("df_my_spore_evn", df_base);

    /*
    ͨ��add_context��ӻ������������ص�Ӧ����share_prt<>��ʹ�û��������ĺô��������Ա�ϵ�л������ݻ���ͬ������һ��cell
    */
    //�ַ�����ʽ�Ļ�������,��Щ�����������Ա���spore����
    auto ctx_root_base = b_root.add_context("fileInfo", "{filename:string; duration:unsigned int; postion: unsigned int; size : {width:int; height:int;}; state:int}");
    //����
    ctx_root_base = b_root.add_context("fileInfo", df_base);
    //����
    ctx_root_base = b_root.add_context("fileInfo", nest.format("df_my_spore_evn"));

    //�Զ������ͻ�������
    auto ctx_root_av = b_root.add_context<av_context>("AVInfo");
    //����
    ctx_root_av = b_root.add_context("AVInfo", df_av);

    /*
    ����ִ������ ���Ӹ�cell��spore���ĳ����ֳ���Ҫ��ִ����
    */
    auto act_demux = b_root.alloc_actuator(1, "act_demux");

    //������pin���������������Pack
    auto pin_out_avpack = b_root.add_out_pin("out_avpack", 
        df_avFrame
        );

    //������pin���������������������Ŀ�������
    auto pin_out_next = b_root.add_out_pin("out_next", 
        df_cmd
        );

    //���һ�����Pin���ڲ��Ļ��������仯ʱ���
    /*
    ��ָ�����ݸ�ʽʱ������ʹ�á�=�����󶨵�����������ʹ�á�@����ָ�������������磺
    "{state:int = %env%.state@after; msg:string;}"
    "{%env%.state@after; msg:string;}"
    */
    b_root.add_out_pin("out_env_changed",
        "{state:int = %env%.state@after; msg:string;}",//��%env%.state�������仯�󡱻�ִ�к�����funtion
        [](const CellContext &context, const ValueMark &mark, const DataPackage &pack)
    {
        pack["msg"] = "the state is changed";
    }
    );

    //�������Pin��ͬʱ�����ʽ��ִ�к�����Ĭ��ִ����
    //���Pin��������ļ�����
    auto pin_in_file_name = b_root.add_in_pin("in_file_name",
        "{filename:string;}",
        [&](const CellContext &context, const DataPackage &pack)
    {
        //��ʼ��av���뻷��������������û�������

        //����ͨ��context���������ʻ���
        context["fileInfo"]["filename"] = pack["filename"].asString();
        context["fileInfo"]["duration"] = 1000;
        //Ҳ����ͨ����ʾ�����������ʻ���
        ctx_root_av.code = NULL;
        //������ʾת��Ϊ�Զ��������
        auto ctx_av = context.as<av_context>("AVInfo");
        /*
        �����"[]"���ʷ�ʽ�п��ܷ��ص��ǲ����ڵĻ�����������ʱ��context�ṹ��һ���ն�������ֵ�������׳��쳣��
        */
        ctx_av.av = NULL;
    }
    );

    //�������Pin�����ڿ��ƽ��, ʹ�ö�����ִ����
    auto pin_in_control = b_root.add_in_pin("in_control",
        df_cmd,
        [&](const CellContext &context, const DataPackage &pack)
    {
        //����ͨ����������ƽ�����
        switch(pack["cmdCode"].asInt())
        {
        case 0: //��ʼ���
            {
                //��ȡ�����Frame
                //...
                //...
                //ͨ�����Pin����һ��packbuilder
                auto b_pack =  pin_out_avpack.pack_builder();
                //���builder
                //...
                //...
                //�����
                pin_out_avpack.add_pack(b_pack);

                /*
                �жϵ�ǰ��state�����Ƿ���Ҫֹͣ��������һ��cmdCode=0�Ŀ��ư����Լ���Ŀ���ǹ���һ����ѭ���������ַ�ʽ��
                1.ֱ��ͨ���Լ���Pin��pin_in_file_name������context��ͨ������ȡ��Pin������������������
                2.�ٹ���һ�����Pin����������pin��Ȼ�����ⲿ�������Pin���Լ���Pin��
                ��1�ַ�ʽ�൱����������ѭ�����߼������������ɵ�һ��pack��������������Ȼ���Լ������Ƿ�ѭ����
                ��2�ַ�ʽ��ѭ�����Ƶ�Ȩ�������ⲿ�������ⲿ���Թһ�������Spore��������

                */
                //��һ�ַ�ʽ
                auto b_next_cmd =  pin_out_next.pack_builder();
                b_next_cmd["cmdCode"] = 0;
                pin_out_next.add_pack(b_next_cmd);
                //�ڶ��ַ�ʽ
                auto b_next_cmd0 =  pin_in_control.pack_builder();
                b_next_cmd0["cmdCode"] = 0;
                pin_in_control.add_pack(b_next_cmd0);
                //����
                auto _pin_out_next = context.find(fs::_meta_pin_type::_LEFT, "in_control");
                if(_pin_out_next){
                    _pin_out_next.add_pack(b_next_cmd0);
                }
            }
        case 1:break; //ֹͣ
        case 2:break; //����
        case 3:break;
        }
        //���»���״̬
        context["state"] = pack["cmdCode"].asInt();
    },
        act_demux
        );

    /*�����ǽ������Ⱦ��spore���壬������������һ��spore�����Ϊ������п����ж��������ÿ�������Ƕ���Ҫ����������ִ�������������Ⱦ��
    ����������һ��spore�����һ�����Ľ������Ⱦ�����ж����ʱ����̬�Ĵ������spore��cell
    */
    typedef fs::SporeBuilder<fs::FlyingSpore> OrgSpore;
    class AVStreamSpore : public OrgSpore{
    private:

    public:
        AVStreamSpore(){
            auto act_decode = alloc_actuator(1, "act_decode");
            auto act_decode = alloc_actuator(1, "act_render");

        }
    }


    //ע��spore
    nest.reg_spore("RootSpore", b_root);
    //ִ��
    nest.run(b_root);
}

#endif // DEMO_H_INCLUDED