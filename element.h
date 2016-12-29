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
    
    class virtual_t;

    class cluster;
    class data_trigger;
    class data_format;

    //数值定位符号，类似于: fileInfo/state这样的路径
    class data_format_path;

    class data_pack;
    class data_context;
    class data_pack_builder;

    class chain;
    class axon;
    class spore;
    class spore_odd;
    class actuator;
    class neure;
    class data_action;

    
    typedef std::shared_ptr<virtual_t> p_virtual_t;

    typedef std::shared_ptr<data_format> p_data_format_t;

    typedef std::unique_ptr<data_pack> p_data_pack_t;
    typedef std::unique_ptr<data_pack_builder> p_data_pack_builder_t;

    typedef std::unique_ptr<chain> p_chain_t;
    typedef std::list<p_chain_t> chain_list_t;
    typedef std::shared_ptr<chain_list_t> p_chain_list_t;


    typedef std::shared_ptr<axon> p_axon_t;

    typedef std::shared_ptr<spore> p_spore_t;
    typedef std::list<p_spore_t> spore_list_t;
    typedef std::shared_ptr<spore_list_t> p_spore_list_t;

    typedef std::shared_ptr<data_context> p_data_context_t;

    typedef std::shared_ptr<actuator> p_actuator_t;

    typedef std::shared_ptr<neure> p_neure_t;

    typedef std::unique_ptr<pack_pool<data_pack>> p_pool_t;

    typedef std::shared_ptr<data_action> p_data_action_t;

    typedef std::function< void(data_context &, const axon &, const data_pack & )> data_handler;
    typedef std::function< bool(const std::string&, const p_axon_t& )> walk_axon_function;

    class virtual_t{
    };

    template<class T> 
    struct deleter{
        inline void operator()(virtual_t* _p)  {
            delete static_cast<T*>(_p);  
        } 
    };

    class data_format{
    public:
    };

    template<typename _struct_t = std::string>
	class data_format {

        static p_data_format_t get_unknow(){
            return p_data_format_t();
        }

		p_data_format_t&& to_data_format(){
            return std::move(p_data_format_t());
        }
	};

    typedef data_format<std::string> format_string;

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

    /*
    链，两端是axon，一个in， 一个out
    */
    class chain{
    public:
        
        chain():_is_valid(false){
        }

        chain(p_axon_t _a1, p_axon_t _a2):_is_valid(false){
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
        p_axon_t _axon_in;
        p_axon_t _axon_out;
        bool _is_valid;
    };

    /*
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
    */

    /*
    neure 一组以确定方式聚合的axon或者子neure
    */
    enum neure_type{_axon_list, _chain_list, _addition, _multiplication};
    class neure{
    public:
        neure(){

        }
        neure(const neure& _other):_type(_other._type),_vector(_other._vector){
        }
        neure(neure&& _other):_type(_other._type){
            std::swap(_vector,_other._vector);
        }
        neure(neure_type _t, const std::vector<p_neure_t>& _ns):_type(_t){
            for(p_neure_t &_p : _ns){
                if(_p)
                    _vector.push_back(std::shared_ptr<virtual_t>(reinterpret_cast<virtual_t>(_p.get()), deleter<neure>()));
            }
        }
        neure(const std::vector<p_axon_t>& _as):_type(neure_type::_axon_list){
            for(p_neure_t &_a : _as){
                if(_a)
                    _vector.push_back(std::shared_ptr<virtual_t>(reinterpret_cast<virtual_t>(_a.get()), deleter<axon>()));
            }
        }
        neure(const std::vector<p_chain_t>& _cs):_type(neure_type::_chain_list){
            for(p_neure_t &_c : _cs){
                if(_c)
                    _vector.push_back(std::shared_ptr<virtual_t>(reinterpret_cast<virtual_t>(_c.get()), deleter<chain>()));
            }
        }

        neure_type get_type() const {
            return _type;
        }

        bool is_empty() const {
            return _vector.empty();
        }
    protected:
        neure_type _type;
        std::vector<p_virtual_t> _vector;
    }

    /*
    链接矩阵，把out neure链接到in neure，生成一组追加的链，然后通过这些链生成chain neure
    */
    class matrix
    {

    public:
        
        void matrix(){

        }
        void matrix(const matrix& _c):_p_spore_list(_c._p_spore_list),_in_neure(_c._in_neure),_out_neure(_c._out_neure){
        }
        void matrix(matrix&& _c):_in_neure(std::move(_c._in_neure)),_out_neure(std::move(_c._out_neure)){
            std::swap(_p_spore_list, _c._p_spore_list);
        }
        void matrix(p_spore_t  _p_s){
            if(_p_s){
                std::vector<p_axon_t> _is;
                std::vector<p_axon_t> _os;
                _p_s->walk_axon([&](const std::string& _name, const p_axon_t & _p){
                    if(_p){
                        if(_p->get_type() == IN_AXON){
                            _is.push_back(_p);
                        }else if(_p->get_type() == OUT_AXON){
                            _os.push_back(_p);
                        }
                    }
                });
                _in_neure = std::make_shared<neure>(_is);
                _out_neure = std::make_shared<neure>(_os);
            }
        }
        void matrix(spore&& _s) == delete;

        void matrix(p_axon_t _p_a){
            if(_p_a){
                if(_p_a->get_type() == IN_AXON)
                    _in_neure = std::make_shared<neure>({_p_a});
                else if(_p_a->get_type() == OUT_AXON)
                    _out_neure = std::make_shared<neure>({_p_a});
            }
        }

        void matrix(axon&& _a) == delete;

        matrix&& operator*(const matrix &_other){
            return std::move(_union(_other, neure_type::_multiplication));
        }

        matrix&& operator+(const matrix &_other){
            return std::move(_union(_other, neure_type::_addition));
        }

        matrix&& operator>>(const matrix &_other){
            //_other 必须包含输入
            if(!_other._in_neure || _other._in_neure->is_empty()){
                //MSG...
                return matrix();
            }
            //这里生成链，再把链封装成neure，追加的out axon里
            
            return matrix();
        }
    protected:
        matrix&& _union(const matrix &_other, neure_type _type){
            matrix _m;
            _m._p_spore_list = std::make_shared<spore_list_t>();
            if(_other._p_spore_list)
                _m._p_spore_list->assign(_other._p_spore_list->begin(), _other._p_spore_list->end());
            if(_p_spore_list)
                _m._p_spore_list->assign(_p_spore_list->begin(), _p_spore_list->end());
            _m._in_neure = std::make_shared<neure>(_type, {_in_neure, _other._in_neure});
            _m._out_neure = std::make_shared<neure>(_type, {_out_neure, _other._out_neure});
            return std::move(_m);
        }

    protected:
        p_spore_list_t _p_spore_list{nullptr};
        p_neure_t _in_neure{nullptr};
        p_neure_t _out_neure{nullptr};
    };


    class axon_builder;
    class axon{
    public:
        enum  signal{data_in};

        data_pack_builder&& get_data_pack_builder(){
            std::move(data_pack_builder());
        }

        void push(data_pack&& _data_pack){

        }
        void push(const data_pack& _data_pack){

        }

        _meta_axon_type get_type() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _type;
        }

        p_data_format_t get_data_format() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _data_format;
        }

        p_actuator_t get_actuator() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _actuator;
        }

        matrix&& connect(p_axon_t &_other){
            if(_type != ~_other->_type)
            {
             //MSG... ERROR
                return std::move(matrix());
            }
            if(_type == _meta_axon_type::IN_AXON)
                return matrix();
            return std::move(matrix({std::make_shared<chain>(shared_ptr<axon>(this) , _other)}));
        }

        matrix&& operator<<(p_axon_t &_other){
            return std::move(matrix(shared_ptr<axon>(this)) << matrix(_other));
        }

        matrix&& operator>>(p_axon_t &_other){
            return std::move(matrix(shared_ptr<axon>(this)) >> matrix(_other));
        }

    protected:
        friend class cluster;
        friend class spore;
        friend class axon_builder;

        _meta_axon_type _type{UNKNOWN};
        p_data_format_t _data_format{nullptr};
        p_actuator_t _actuator{nullptr};
    };

    struct data_action{
        data_action(const data_trigger _t, const data_handler &_h):_trigger(_t), _handler(_h){
        }
        data_trigger _trigger;
        data_handler _handler;
    };

    class axon_builder : public axon{
    public:
        void reset(){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.clear();
            _type = _meta_axon_type::UNKNOWN;
            _data_format = nullptr;
            _actuator = nullptr;
        }

        void set_type(_meta_axon_type _t){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _type = _t;
        }

        void set_data_format(p_data_format_t _f){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _data_format = _f;
        }

        void set_actuator(const p_actuator_t& _act){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _actuator = _act;
        }

        void add_action(const data_trigger& _trigger, const data_handler& _action){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.push_back(std::make_shared<data_action>(_trigger, _actuator));
        }

        void add_action(axon::signal _signal, const data_handler& _action){
            add_action(data_trigger(this, _signal), std::move(_action));
        }

        p_axon_t to_axon() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            p_axon_t _a = std::make_shared<axon>();
            _a->_type = _type;
            _a->_data_format = _data_format;
            _a->_actuator = _actuator;
            return _a;
        }

    protected:
        mutable std::mutex _lock_builder;
        std::list<p_data_action_t> _action_list;
    };


    class neure{
    public:
    };


    /*
    //typedef std::function< bool(const p_axon_t & )> walk_axon_function;
    class spore{
    public:
        spore(){
        }
        //void walk_axon(_meta_axon_type _type, const walk_axon_function &_func){
        //    if(!_func) return;
        //    for (std::vector<p_axon_t>::const_iterator ite = _axon_array.begin(); ite != _axon_array.end(); ite ++){
        //        if(!(_func)(*ite))
        //            return;
        //    }
        //}
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
    */

    class spore_builder;
    class spore{
    public:
        spore(){

        }

        p_spore_t clone(){
            return p_spore_t();
        }

        p_spore_t newborn(){
            return p_spore_t();
        }

        void walk_axon(const walk_axon_function &_func){
            if(!_func) return;
            for (auto ite = _axon_array.begin(); ite != _axon_array.end(); ite ++){
                if(!(_func)(ite->first, ite->second))
                    return;
            }
        }
    protected:
        std::map<std::string, p_data_format_t> _context_array;
        std::map<std::string, p_axon_t> _axon_array;
        std::list<p_data_action_t> _action_list;
    };

    class spore_builder: public spore{
    public:
        p_data_format_t add_data_format(std::string _format_name,  const p_data_format_t& _p_data_format_t){
            if(!_p_data_format_t)
                return nullptr;
            p_data_format_t _p{nullptr};
            std::lock_guard<std::mutex> lk(_lock_builder);
            _context_array[_format_name] = _p = std::make_shared<data_format>(*_p_data_format_t->get());
            return _p;
        }
        p_axon_t add_axon(std::string _axon_name, const axon_builder& _axon_builder){
            p_axon_t _p = _axon_builder.to_axon();
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.merge(_axon_builder._action_list);
            _axon_array[_axon_name] = _p;
            return _p;
        }

        void add_trigger(const data_trigger& _trigger, const data_handler& _action){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.push_back(std::make_shared<data_action>(_trigger, _actuator));
        }
    protected:
        std::mutex _lock_builder;
    };


    class spore_odd : public spore{
        p_axon_t _out_axon;
        p_axon_t _in_axon;
    public:
        spore_odd(){
            _out_axon.set_type(_meta_axon_type::OUT);
            _out_axon.set_data_format(data_format::get_unknow());
            p_axon_t _out_axon = add("_out", meta_axon_type::OUT);
            
            _in_axon.set_type(_meta_axon_type::IN_AXON);
            _in_axon.set_data_format(data_format::get_unknow());
            _in_axon.add_trigger(axon::signal::data_in,
                [&](data_context &context, const axon &_axon, const data_pack &_in_pack){
                    _out->push(_in_pack);
            });
            add("_in", _in_axon);
        }
    };



    class data_trigger{
    public:
        /*
        1. handel, 用于描述触发这个trigger的来源
        2. signal, 描述这个源的什么信号会导致触发
        */
        /*for data_pack*/
        data_trigger(const data_pack& _data_pack, data_format_path&& _path, data_pack::signal _signal){
        }
        /*for axon*/
        data_trigger(const p_axon_t& _axon, axon::signal _signal){

        }

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
                _p_neure_builder = std::make_shared<neure>();
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
        p_neure_t _p_neure_builder;
        std::once_flag _init_pool_flag;
        std::once_flag _init_neure_builder_flag;
        std::mutex _lock_operate;
        status_t _status{status_t::original};
    };

}





#endif // ELEMENT_H_INCLUDED