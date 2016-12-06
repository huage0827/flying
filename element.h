/*!
 * \file element.h
 * \date 2016/12/06 18:05
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: prototype of flying spore 
 *
 * \note
*/

#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED
#include "core.h"
#include "pack_pool.h"

namespace sf
{

    class cluster;
    class trigger;
    class data_format;
    class data_pack;
    //��ֵ��λ���ţ�������: fileInfo/state������·��
    class data_format_path;
    class axon;
    class spore;
    class cell;
    class data_context;
    class actuator;
    class neure;
    class actuator;

    typedef std::function< void(data_context &, const axon &, const data_pack & )> trigger_action;

	class data_format_builder {

		data_format&& to_data_format();
	};

	class data_pack_builder {
		data_pack&& to_data_pack();
	};

    class data_pack
    {
    public:
        enum signal{before_changed,  after_changed};
    };


    class data_context : public data_pack{

    };

    typedef std::function< bool(const axon & )> walk_axon_function;
    class spore{
    public:

        void walk_axon(_meta_axon_type _type, const walk_axon_function &_func);

    protected:
        friend class cluster;
    };

    class spore_builder{
     /*
        ͨ��add_trigger�ķ�ʽ���Ӵ������ƣ�trigger����һ��trigger������
        1. handel, ���������������trigger����Դ
        2. signal, �������Դ��ʲô�źŻᵼ�´���
        3. function, ����Ĵ���ִ�к���
        
        add_trigger ��һ�����أ�ģ�壿��������ͨ��handel��signal�����Ϳ��Ծ���function������
    */
        void add_trigger(trigger_type _type, const trigger_name _name,  trigger&& _trigger, trigger_action&& _action);

		spore&& to_spore();
    };


    class axon
    {
    public:
        enum  signal{data_in};

        data_pack_builder& get_data_pack_builder();
        void push(data_pack&& _data_pack);
    protected:
        friend class cluster;
        friend class spore;
    };

    class axon_builder{

        void add_trigger(trigger_type _type,  trigger&& _trigger, trigger_action&& _action){

        }

        void add_trigger(trigger_type _type,  axon::signal _signal, trigger_action&& _action){
            add_trigger(_type, trigger(&this, _signal), std::move(_action));
        }

		axon&& to_axon();
    };

    class trigger{
    public:
    public:
        enum trigger_type{fixed, can_replace};

        /*for data_pack*/
        trigger(const data_pack& _data_pack, data_format_path&& _path, data_pack::signal _signal);
        /*for axon_builder*/
        trigger(const axon_builder& _axon_builder, axon::signal _signal);
        /*for axon*/
        trigger(const axon& _axon, axon::signal _signal);

    };

    class cluster{
    public:
        enum ability_t{
            anonymous = 1 << 0,
            visible =          1 << 1, 
            query =           1 << 2, 
            transfer =        1 << 3, 
            performer =    1 << 4,
            dispatcher =    1 << 5,
            master =         1 << 6,
            sages =            visible | query | transfer | performer | dispatcher | master,
        };
        cluster(const cluster&) = delete;
        cluster(const cluster&&) = delete;
        cluster& operator=(const cluster&) = delete;
        cluster(unsigned int actuator_max_count = std::thread::hardware_concurrency() + 1,
            ability_t ability = ability_t::anonymous):
        _actuator_max_count(actuator_max_count),_ability(ability),_actuator_alloc_count(0){
        }

        void run(const std::vector<spore> &_spores){
            init_pool();

        }
    protected:
        void pack_action(data_pack& _pack){

        }

        data_pack&& fill_empty_pack(){
            std::move(data_pack());
        }

        void init_pool(){
            std::call_once(_init_pool_flag, [&](){
                unsigned int base_thread_count = _actuator_max_count - _actuator_alloc_count;
                if(base_thread_count > 0){
                    _p_pool = std::make_unique<pack_pool<data_pack>>(
                        std::bind(&cluster::pack_action, this),
                        std::bind(&cluster::fill_empty_pack, this),
                        base_thread_count
                        );
                }
            });
        }
    protected:
        const unsigned int _actuator_max_count;
        const ability_t _ability;
        unsigned int _actuator_alloc_count;

        std::unique_ptr<pack_pool<data_pack>> _p_pool;
        std::once_flag _init_pool_flag;
    };

}





#endif // ELEMENT_H_INCLUDED