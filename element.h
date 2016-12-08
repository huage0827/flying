/*!
 * \file element.h
 * \date 2016/12/06 18:05
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * 本文档是"flying spore" 原型设计草案的一部分，它在框架层面描述应该提供哪些API给使用者。 
 *
 * \note
*/

#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED
#include <mutex>
#include "core.h"
#include "pack_pool.h"

namespace sf
{

    class cluster;
    class trigger;
    class data_format;
    class data_format_builder;

    //数值定位符号，类似于: fileInfo/state这样的路径
    class data_format_path;

    class data_pack;
    class data_context;
    class data_pack_builder;

    class axon;
    class axon_builder;

    class spore;
    class spore_builder;

    class actuator;

    class neure;
    class neure_builder;

    typedef std::shared_ptr<data_format> p_data_format_t;
    typedef std::shared_ptr<data_format_builder> p_data_format_builder_t;

    typedef std::unique_ptr<data_pack> p_data_pack_t;
    typedef std::unique_ptr<data_pack_builder> p_data_pack_builder_t;

    typedef std::shared_ptr<axon> p_axon_t;
    typedef std::shared_ptr<spore> p_spore_t;
    typedef std::shared_ptr<spore_builder> p_spore_builder_t;
    typedef std::shared_ptr<data_context> p_data_context_t;
    typedef std::shared_ptr<neure> p_neure_t;
    typedef std::shared_ptr<neure_builder> p_neure_builder_t;

    typedef std::unique_ptr<pack_pool<data_pack>> p_pool_t;

    typedef std::function< void(data_context &, const axon &, const data_pack & )> trigger_action;

    class data_format{

    };

    template<typename _struct_t = std::string>
	class data_format_builder {

		p_data_format_t&& to_data_format();
	};

    typedef data_format_builder<std::string> format_string;

    class data_value{

    };

	class data_pack_builder {
		data_pack&& to_data_pack();
	};

    class data_pack
    {
    public:
        enum signal{before_changed,  after_changed};

        const data_value& operator[](std::string _path){
            return data_value();
        }

        const data_value& operator[](char* _path){
            if(!_path)
                return data_value();
            return (*this)[std::string(_path)];
        }

    protected:
    };


    class data_context : public data_pack{

    };

    typedef std::function< bool(const axon & )> walk_axon_function;
    class spore{
    public:

        void walk_axon(_meta_axon_type _type, const walk_axon_function &_func);
        unsigned int get_actuator_alloc_count(){
            return 0;
        }
    protected:
        friend class cluster;
        friend class spore_builder;
        cluster *_p_cluster{nullptr};
    };

    class spore_builder{
    public:

        void reset();

        p_data_format_t&& add(char* _format_name, const p_data_format_t& _data_format){
            if(_format_name)
                return add(std::string(_format_name), _data_format);
            return nullptr;
        }
        p_data_format_t&& add(std::string _format_name, const p_data_format_t& _data_format){

            return reg(_data_format);
        }

        p_axon_t&& add(char* _axon_name, const p_axon_t& _axon){
            if(_axon_name)
                return add(std::string(_axon_name), _axon);
            return nullptr;
        }
        p_axon_t&& add(std::string _axon_name, const p_axon_t& _axon){

            return std::move(p_axon_t(_axon));
        }

        p_data_format_t&& reg(const p_data_format_t& _data_format){
            
            //reg ...
            
            return std::move(p_data_format_t(_data_format));
        }
     /*
        通过add_trigger的方式添加处理机制（trigger），一个trigger包括：
        1._trigger, trigger
        2._action, 具体的处理执行函数
     */
        void add_trigger(trigger&& _trigger, trigger_action&& _action);

		spore&& to_spore();
    protected:

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
        friend class axon_builder;
        unsigned int _actuator_id;
    };

    class axon_builder{
    public:

        void set_type(_meta_axon_type _t){
            _type = _t;
        }
        _meta_axon_type get_type(){
            return _type;
        }

        void set_data_format(p_data_format_t _f){
            _data_format = _f;
        }
        p_data_format_t get_data_format(){
            return _data_format;
        }

        void set_actuator(const actuator& _act){
            _actuator = _act;
        }
        actuator&& get_actuator(){
            return std::move(actuator(_actuator));
        }

        void reset();

        void add_trigger(trigger&& _trigger, trigger_action&& _action){

        }

        void add_trigger(axon::signal _signal, trigger_action&& _action){
            add_trigger(trigger(&this, _signal), std::move(_action));
        }

		p_axon_t&& to_axon();
    protected:
        _meta_axon_type _type{UNKNOWN};
        p_data_format_t _data_format{nullptr};
        actuator _actuator;
    };

    class trigger{
    public:
        /*
        1. handel, 用于描述触发这个trigger的来源
        2. signal, 描述这个源的什么信号会导致触发
        */
        /*for data_pack*/
        trigger(const data_pack& _data_pack, data_format_path&& _path, data_pack::signal _signal);
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
        enum status_t{
            original,
            initialize,
            running,
            releasing,
            finish,
            fail
        };
        cluster(const cluster&) = delete;
        cluster(const cluster&&) = delete;
        cluster& operator=(const cluster&) = delete;
        cluster(unsigned int actuator_max_count = std::thread::hardware_concurrency() + 1,
            ability_t ability = ability_t::anonymous):
        _actuator_max_count(actuator_max_count),_ability(ability){
        }

        bool run(const std::vector<spore> &_spores){
            std::lock_guard<std::mutex> lk(_lock_operate);
            if(_status != status_t::original)
                return false;
            _status = status_t::initialize;

            //reg all spore....

            init_neure_builder();
            //scanning
            _actuator_alloc_count = 0;
            std::for_each(_spores.begin(), _spores.end(), [&](spore &_spore){
                _spore._p_cluster = this;
                _actuator_alloc_count += _spore.get_actuator_alloc_count();
                
                //distribute  the neure builder

            });
            init_pool();
            if(_actuator_max_count - _actuator_alloc_count < 1){
                _status = status_t::fail;
                return false;
            }
            return true;
        }

        p_spore_t&& reg(std::string _name, p_spore_t _spore){
            if(!_spore)
                return nullptr;

            //reg spore

            return std::move(p_spore_t(_spore));
        } 

        p_data_format_t&& reg(std::string _name, p_data_format_t _format){
            if(!_spore)
                return nullptr;

            //reg data format

            return std::move(p_data_format_t(_format));
        }

    protected:
        void pack_action(data_pack& _pack){
            //push to pack to list
        }

        data_pack&& fill_empty_pack(){
            std::move(data_pack());
        }

        void init_neure_builder(){
            std::call_once(_init_neure_builder_flag, [&](){
                _p_neure_builder = std::make_shared<neure_builder>();
            });

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
        const unsigned int _actuator_max_count{0};
        const ability_t _ability{ability_t::anonymous};
        unsigned int _actuator_alloc_count{0};

        p_pool_t _p_pool;
        p_neure_builder_t _p_neure_builder;
        std::once_flag _init_pool_flag;
        std::once_flag _init_neure_builder_flag;
        std::mutex _lock_operate;
        status_t _status{status_t::original};
    };

}





#endif // ELEMENT_H_INCLUDED