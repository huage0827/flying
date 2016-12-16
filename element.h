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

    class chain;
    class chain_builder;

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

    typedef std::unique_ptr<chain> p_chain_t;
    typedef std::shared_ptr<chain_builder> p_chain_builder_t;


    typedef std::shared_ptr<axon> p_axon_t;
    typedef std::shared_ptr<axon_builder> p_axon_builder_t;

    typedef std::shared_ptr<spore> p_spore_t;
    typedef std::shared_ptr<spore_builder> p_spore_builder_t;

    typedef std::shared_ptr<data_context> p_data_context_t;

    typedef std::shared_ptr<actuator> p_actuator_t;

    typedef std::shared_ptr<neure> p_neure_t;
    typedef std::shared_ptr<neure_builder> p_neure_builder_t;

    typedef std::unique_ptr<pack_pool<data_pack>> p_pool_t;

    typedef std::function< void(data_context &, const axon &, const data_pack & )> trigger_action;

    class data_format{
    public:
    };

    template<typename _struct_t = std::string>
	class data_format_builder {

		p_data_format_t&& to_data_format();
	};

    typedef data_format_builder<std::string> format_string;

    class data_value{
    public:
        bool as_bool();
        char as_char();
        unsigned char as_byte();
        short as_short();
        long as_long();
        long long as_longlong();
        std::string as_string();
    };

	class data_pack_builder {
    public:
		data_pack&& to_data_pack();
	};

    class data_pack
    {
    public:
        enum signal{before_changed,  after_changed};

        const data_value& operator[](std::string _path){
            //mapping path to index and find it
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
    public:
    protected:
        p_spore_t _owner;
    };

    class chain_builder{
    public:
        
        chain_builder():_is_valid(false){
        }

        chain_builder(p_axon_builder_t _a1, p_axon_builder_t _a2):_is_valid(false){
            if(_a1->_type == ~_a2->_type){
                //类型匹配判断
                //...
                //...
                if(_a1->_type == _meta_axon_type::IN_AXON)
                    _axon_in = _a1;
                else
                    _axon_out = _a1;
                if(_a2->_type == _meta_axon_type::IN_AXON)
                    _axon_in = _a2;
                else
                    _axon_out = _a2;
            }
        }

        p_chain_t&& to_chain(){
            return std::move(p_chain_t());
        }

        bool is_valid(){
            return _is_valid;
        }

    protected:
        p_axon_builder_t _axon_in;
        p_axon_builder_t _axon_out;
        bool _is_valid;
    };

    class matrix{
    public:
        void matrix(){}

        void matrix(axon_builder &_axon){
            add({p_axon_builder_t(&_axon)});
        }
        void matrix(std::vector<p_axon_builder_t> &_axons){
            add(_axons);
        }
        void matrix(std::vector<p_axon_builder_t> &&_axons){
            add(std::move(_axons));
        }
        void matrix(std::vector<p_chain_builder_t> &_chains){
            add(_chains);
        }
        void matrix(std::vector<p_chain_builder_t> &&_chains){
            add(std::move(_chains));
        }

        void add(std::vector<p_axon_builder_t> &_axons){
            _meta_axon_type _t = _meta_axon_type::UNKNOWN;
            for_each(_axons.begin(), _axons.end(),  [&](p_axon_builder_t &_axon){
                if(_t != _meta_axon_type::UNKNOWN && _axon->_type != _t){
                    _t = _meta_axon_type::UNKNOWN;
                    break;
                }
                _t = _axon->_type;
            });
            if(_t == _meta_axon_type::IN_AXON)
                _axons_in.push_back(_axons);
            else if(_t == _meta_axon_type::OUT_AXON)
                _axons_out.push_back(_axons);
        }
        void add(std::vector<p_axon_builder_t> &&_axons){
            _meta_axon_type _t = _meta_axon_type::UNKNOWN;
            for_each(_axons.begin(), _axons.end(),  [&](p_axon_builder_t &_axon){
                if(_t != _meta_axon_type::UNKNOWN && _axon->_type != _t){
                    _t = _meta_axon_type::UNKNOWN;
                    break;
                }
                _t = _axon->_type;
            });
            if(_t == _meta_axon_type::IN_AXON)
                _axons_in.emplace_back(_axons);
            else if(_t == _meta_axon_type::OUT_AXON)
                _axons_out.emplace_back(_axons);
        }
        void add(std::list<p_chain_builder_t> &_chains){
            _chains.push_back(_chains);
        }
        void add(std::list<p_chain_builder_t> &&_chains){
            _chains.emplace_back(_chains);
        }

    protected:
        std::list<std::vector<p_axon_builder_t>> _axons_in;
        std::list<std::vector<p_axon_builder_t>> _axons_out;
        std::list<p_chain_builder_t> _chains;
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
            std::lock_guard<std::mutex> lk(_lock_builder);
            _type = _t;
        }
        _meta_axon_type get_type(){
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _type;
        }

        void set_data_format(p_data_format_t _f){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _data_format = _f;
        }
        p_data_format_t get_data_format(){
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _data_format;
        }

        void set_actuator(const p_actuator_t& _act){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _actuator = _act;
        }
        actuator&& get_actuator(){
            std::lock_guard<std::mutex> lk(_lock_builder);
            return std::move(actuator(_actuator));
        }

        void reset(){

        }

        void add_trigger(const trigger& _trigger, trigger_action&& _action){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _trigger_action[_trigger] = std::move(_action);
        }

        void add_trigger(const axon::signal _signal, trigger_action&& _action){
            add_trigger(trigger(&this, _signal), std::move(_action));
        }

        p_axon_t&& to_axon(){
            std::lock_guard<std::mutex> lk(_lock_builder);

            return std::move(p_axon_t());
        }

        matrix&& connect(p_axon_builder_t &_other){
            if(_type != ~_other->_type)
            {
             //MSG... ERROR
                return std::move(matrix());
            }
            if(_type == _meta_axon_type::IN_AXON)
                return 
            return std::move(matrix({std::make_shared<chain_builder>(shared_ptr<axon_builder>(this) , _other)}));
        }

        matrix&& operator<<(p_axon_builder_t &_other){
            return std::move(matrix(shared_ptr<axon_builder>(this)) << matrix(_other));
        }

        matrix&& operator>>(p_axon_builder_t &_other){
            return std::move(matrix(shared_ptr<axon_builder>(this)) >> matrix(_other));
        }

    protected:
        std::mutex _lock_builder;
        _meta_axon_type _type{UNKNOWN};
        p_data_format_t _data_format{nullptr};
        p_actuator_t _actuator;
        std::map<trigger, trigger_action> _trigger_action;
    };

    typedef std::function< bool(const p_axon_t & )> walk_axon_function;
    class spore{
    public:
        spore(){
        }
        void walk_axon(_meta_axon_type _type, const walk_axon_function &_func){
            if(!_func) return;
            for (std::vector<p_axon_t>::const_iterator ite = _axon_array.begin(); ite != _axon_array.end(); ite ++){
                if(!(_func)(*ite))
                    return;
            }
        }
        unsigned int get_actuator_alloc_count(){
            return 0;
        }
    protected:
        friend class cluster;
        friend class spore_builder;
        cluster *_p_cluster{nullptr};
        std::vector<p_axon_t> _axon_array;
        //p_axon_t _axon_array{nullptr};//=shared_ptr<axon>(new axon[size], default_delete<axon[]>()
        
    };

    class spore_builder{
    public:

        void reset();

        data_format_builder& add(char* _format_name, data_format_builder& _data_format_builder){
            if(_format_name)
                return add(std::string(_format_name), _data_format);
            return _data_format_builder;
        }
        data_format_builder& add(std::string _format_name, data_format_builder& _data_format_builder){
            {
                std::lock_guard<std::mutex> lk(_lock_builder);
                if(std::find(_context_array.begin(), _context_array.end(), _format_name) != _context_array.end()){
                    //MSG...
                    return _data_format_builder;
                }
                _context_array[_format_name] = _data_format_builder;
            }
            return reg(_data_format_builder);
        }

        axon_builder& add(char* _axon_name, axon_builder& _axon_builder){
            if(_axon_name)
                return add(std::string(_axon_name), _axon_builder);
            return _axon_builder;
        }
        axon_builder& add(std::string _axon_name, axon_builder& _axon_builder){
            {
                std::lock_guard<std::mutex> lk(_lock_builder);
                if(std::find(_axon_array.begin(), _axon_array.end(), _axon_name) != _axon_array.end()){
                    //MSG... ERROR
                    return _axon_builder;
                }
                _axon_array[_axon_name] = _axon_builder;
            }
            return _axon_builder;
        }

        data_format_builder& reg(data_format_builder& _data_format_builder){
            
            //reg ...
            
            return _data_format_builder;
        }
     /*
        通过add_trigger的方式添加处理机制（trigger），一个trigger包括：
        1._trigger, trigger
        2._action, 具体的处理执行函数
     */
        void add_trigger(trigger& _trigger, trigger_action& _action);

		spore&& to_spore();
    protected:
        std::mutex _lock_builder;
        std::map<std::string, data_format_builder> _context_array;
        std::map<std::string, axon_builder> _axon_array;
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