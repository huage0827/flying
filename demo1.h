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

//Ϊroot spore ��ӻ�������
struct AVCode;
struct av_context{
    AVCode code;
};
struct AVFrame;

int __main(){


    //�������л���������


    cluster _cluster;
    //��Ϊһ�����ŵ�Nest�����32��ִ�������߱�ȫ����ִ���������ɱ����֡��ɱ���ѯ������ת��ѯ���ɽ������񡢿ɷ������񡢿ɽ�������Ȩ����ת������Ȩ�����������
    Runtime rt(32, cluster::ability_t::sages);
    _cluster.publish(rt, "127.0.0.1", 6101/*��·�˿�*/, 6201/*ҵ��˿�*/ );
    //������Ϊһ����Ⱥ���ܿ�Nest�����ռ�Ⱥ���õ�spore�����ļ��ڵ�����
    Runtime rt0;//��֧�ֵ�����ʱ������Ĭ��Ϊ�գ����������Nest�����ܺ�������Nest���ӣ��ɷ��ʸ������ļ�����Ļ�������
    _cluster.join(rt0, "https://github.com/kicsy/flying/demo/publish/process/Video_processing.fspd");
    //������Ϊһ����չ��Nest�����뵽��һ��Nest�Ļ��
    _cluster.join(rt, "127.0.0.1"/*��һNest�ĵ�ַ*/, 6102/*��·�˿�*/, 6202/*ҵ��˿�*/ );

    //����һ�� spore builder��ָ������
    auto b_root = spore_builder();

    /*	
    �������ݸ�ʽ
    ���ݸ�ʽ��format�����������֡����ԣ������ǿ����л��ģ�
    �ڲ�����������Ŀ��entry������Ŀ�����֡����͡����ԣ�0-*��
    ��׼���͵���Ŀͨ�����Ʒ���(�ײ���ܵĻ���Ҫӳ���ID������������Щ)
    �Զ������͵Ļ����ڴ��ݹ����п������ͻᶪʧ����ʹ��ʱ������ʾ����ת���������Ͱ�ȫ�Ĳ�����ʽ
    */
    data_format df_base = data_format_builder("df_my_spore_evn", "{filename:string; duration:unsigned int; postion: unsigned int; size{width:int; height:int;}; state:int}").to_data_format();
    data_format df_av = data_format_builder<av_context>("df_av_info").to_data_format();
    data_format df_avFrame = data_format_builder<AVFrame>().to_data_format();
    data_format df_cmd = data_format_builder("{cmdCode:int;seekPos:int(*); }").to_data_format();
    //ע�ᵽ��ʽ����ϵͳ�����Ǳ����
    b_root.reg(df_base);

    /*
    ͨ��add��ӻ���������ʹ�û��������ĺô��������Ա�ϵ�л������ݻ���ͬ������һ��cell
    */
    b_root.add("fileInfo", df_base);
    //�Զ������ͻ�������
    b_root.add("AVInfo", df_av);

    axon_builder _b_axon;
    //���axon���������������Pack
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::OUT);
    _b_axon.set_data_format(df_avFrame);
    axon _axon_out_avpack = b_root.add("out_avpack", _b_axon.to_axon());

    //���axon���������������������Ŀ�������
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::OUT);
    _b_axon.set_data_format(df_cmd);
    axon _axon_out_next = b_root.add("out_next", _b_axon.to_axon());

    //���һ�����axon���ڲ��Ļ��������仯ʱ���
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::OUT);
    _b_axon.set_data_format(data_format_builder("{state:int; msg:string;}").to_data_format());
    //����axon�Ĵ�����ƣ���Ҫ����������������
    _b_axon.add_trigger(trigger::trigger_type::fixed,
        trigger(b_root.context(), data_format_path("fileInfo/state"), data_pack::signal::after_changed),
         //����axon���ʱ�Ĵ�����
        [](data_context &context, const axon &_axon, const data_pack &_in_pack){
            //���_in_pack��ʵ��contextָ����ͬ������
            data_pack_builder _b_data_pack = _axon.get_data_pack_builder();
            _b_data_pack["state"] = context["fileInfo/state"];
            _b_data_pack["msg"] = "the state is changed";
            _axon.push(_b_data_pack.to_data_pack());
        }
    );
    b_root.add("out_env_changed", _b_axon.to_axon());


    //�������axon
    //���axon��������ļ�����
    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::IN);
    _b_axon.set_data_format(data_format_builder("{filename:string;}").to_data_format());
    _b_axon.add_trigger(trigger::trigger_type::fixed,
        axon::signal::data_in,
        [](data_context &context, const axon &_axon, const data_pack &_in_pack){
            //����_in_pack�Ǵӵ�ǰ��axon�����ݣ���axon::signal::data_in�źŴ�����
            //��ʼ��av���뻷��������������û�������

            //����ͨ��context���������ʻ���
            context["fileInfo/filename"] = pack["filename"].asString();
            context["fileInfo/duration"] = 1000;
            //������ʾת��Ϊ�Զ��������
            auto ctx_av = context.as<av_context>("AVInfo");
            /*
            �����"[]"���ʷ�ʽ�п��ܷ��ص��ǲ����ڵĻ�����������ʱ��context�ṹ��һ���ն�������ֵ�������׳��쳣��
            */
            ctx_av.av = NULL;
        }
    );
    b_root.add("in_file", _b_axon.to_axon());

    //�������axon�����ڿ��ƽ��, ʹ�ö�����ִ����
    
    /*
    ����ִ������ ���Ӹ�cell��spore���ĳ����ֳ���Ҫ��ִ����
    */
    actuator act_demux(1, "act_demux");

    _b_axon.reset();
    _b_axon.set_type(_meta_axon_type::IN);
    _b_axon.set_data_format(df_cmd);
    _b_axon.set_actuator(act_demux);
    axon _axon = b_root.add("in_control", _b_axon.to_axon());

    //���ｫ�������axon��ִ�������spore��
    b_root.add_trigger(trigger::trigger_type::fixed,
        trigger(_axon, axon::signal::data_in),
        [&](data_context &context, const axon &_axon, const data_pack &_in_pack){
            //����ͨ����������ƽ�����
            switch(_in_pack["cmdCode"].asInt())
            {
            case 0: //��ʼ���
                {
                    //��ȡ�����Frame
                    //...
                    //...
                    //ͨ�����axon����һ��packbuilder
                    auto b_pack =  _axon_out_avpack.get_data_pack_builder();
                    //���builder
                    //...
                    //...
                    //�����
                    _axon_out_avpack.push(b_pack);

                    /*
                    �жϵ�ǰ��state�����Ƿ���Ҫֹͣ��������һ��cmdCode=0�Ŀ��ư����Լ���Ŀ���ǹ���һ����ѭ���������ַ�ʽ��
                    1.ֱ��ͨ���Լ���Pin��pin_in_file_name������context��ͨ������ȡ��Pin������������������
                    2.�ٹ���һ�����Pin����������pin��Ȼ�����ⲿ�������Pin���Լ���Pin��
                    ��1�ַ�ʽ�൱����������ѭ�����߼������������ɵ�һ��pack��������������Ȼ���Լ������Ƿ�ѭ����
                    ��2�ַ�ʽ��ѭ�����Ƶ�Ȩ�������ⲿ�������ⲿ���Թһ�������Spore��������

                    */
                    //��һ�ַ�ʽ
                    auto b_next_cmd0 =  _axon.get_data_pack_builder();
                    b_next_cmd0["cmdCode"] = 0;
                    _axon.push(b_next_cmd0);
                    //�ڶ��ַ�ʽ
                    auto b_next_cmd =  _axon_out_next.get_data_pack_builder();
                    b_next_cmd["cmdCode"] = 0;
                    _axon_out_next.push(b_next_cmd);
                    }
                }
            case 1:break; //ֹͣ
            case 2:break; //����
            case 3:break;
            }
            //���»���״̬
            context["state"] = _in_pack["cmdCode"];
        }
    );

    //ע��spore
    _cluster.reg("RootSpore", b_root.to_spore());
    /*
    ���ˣ�����ͨ��spore_builder������һ��spore�����spore��ͨ����װ�䡱�����ģ����ɶ���ⲿ������������װ���ɡ���ᷢ�֣���������Ϊһ���ϸ������ϵġ��ࡱ��
    ������װ��һ�������޸ĵ�sporeʵ�壬���ġ����ݡ�ʹ��context�����أ������ⲿ�ӿ���ÿ��axon�����Ĳ�������ÿ��axon�ϵ�trigger��ִ���壬����axon���Ա���ʾ
    Ϊ�����滻������������sporeʵ��ֻ��һ�����ͣ�����spore�������������Ƕ�̬��
    Ψһ�ʹ����಻һ��������û�����Ա�����������ڲ����ݲ�����������context����contextĬ�϶���ԭ�Ӳ����������Ƚϴ�
    ����취��ֱ�Ӽ̳�sporeʵ���µ�spore�࣬����Ȼͨ��spore_builder������ṹ��
    */

    /*
    �����ǽ������Ⱦ��spore���壬������������һ��spore�����Ϊ������п����ж��������ÿ�������Ƕ���Ҫ����������ִ�������������Ⱦ��
    ����������һ��spore�����һ�����Ľ������Ⱦ�����ж����ʱ����̬�Ĵ������spore��cell
    */
    class stream_spore : public spore{
    private:

    public:
        stream_spore(){
			//��ӻ�������
			add("AVInfo", df_av);

			actuator act_decode(1, "act_decode");
			actuator act_render(1, "act_render");

        }
    }

	//��builder����spore
	spore _root = b_root.to_spore();
	stream_spore _stream_spore;
	_stream_spore.context["AVInfo"].sync_by(_root.context["AVInfo"]);

    //ִ��
	_cluster.run({_root, _stream_spore});
}

#endif // DEMO_H_INCLUDED