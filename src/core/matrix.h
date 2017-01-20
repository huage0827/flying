#ifndef flying_spore_matrix_h
#define flying_spore_matrix_h
#include <memory>
#include <vector>
#include "neure.h"

namespace flyingspore{

    class matrix;
    typedef std::shared_ptr<matrix> p_matrix_t;

    /*
    链接矩阵，把out neure链接到in neure，生成一组追加的链，然后通过这些链生成chain neure
    */
    class matrix
    {

    public:
        matrix(){
        }
        matrix(const matrix& _c):_p_in_neure(_c._p_in_neure),_p_out_neure(_c._p_out_neure){
        }
        matrix(matrix&& _c):_p_in_neure(std::move(_c._p_in_neure)),_p_out_neure(std::move(_c._p_out_neure)){
            std::swap(_p_spore_list, _c._p_spore_list);
        }
        matrix(p_spore_t  _p_s){
            if(_p_s){
                object_list_t _is;
                object_list_t _os;
                _p_s->walk_axon([&](const std::string& _name, const p_axon_t & _p)->bool{
                    if(_p){
                        if(_p->get_type() == IN_AXON){
                            _is.push_back(_p);
                        }else if(_p->get_type() == OUT_AXON){
                            _os.push_back(_p);
                        }
                    }
                    return true;
                });
                _p_in_neure = std::make_shared<neure>(neure_type::_multiplication, std::move(_is));
                _p_out_neure = std::make_shared<neure>(neure_type::_multiplication,std::move(_os));
            }
        }

        matrix(p_axon_t _p_a){
            if(_p_a){
                if(_p_a->get_type() == IN_AXON)
                    _p_in_neure = std::make_shared<neure>(neure_type::_multiplication, {dynamic_pointer_cast<base_object>(_p_a)});
                else if(_p_a->get_type() == OUT_AXON)
                    _p_out_neure = std::make_shared<neure>(neure_type::_multiplication, {dynamic_pointer_cast<base_object>(_p_a)});
            }
        }

        matrix(const std::vector<p_axon_t>& _p_as){
            object_list_t _is;
            object_list_t _os;
            for (auto &_a: _p_as){
                if(_a->get_type() == IN_AXON){
                    _is.push_back(_a);
                }else if(_a->get_type() == OUT_AXON){
                    _os.push_back(_a);
                }
            }
            _p_in_neure = std::make_shared<neure>(neure_type::_multiplication, std::move(_is));
            _p_out_neure = std::make_shared<neure>(neure_type::_multiplication,std::move(_os));
        }

        matrix&& operator*(const matrix &_other){
            return std::move(_union(*this, _other, neure_type::_multiplication));
        }

        matrix&& operator+(const matrix &_other){
            return std::move(_union(*this, _other, neure_type::_addition));
        }

        matrix&& operator>>(const matrix &_other){
            //这里生成链，再把链封装成neure，追加的out axon里

            return matrix();
        }

    protected:
        static matrix&& _union(const matrix &_m1, const matrix &_m2, neure_type _type){
            matrix _m;
            if(_m2._p_spore_list || _m2._p_spore_list)
                _m._p_spore_list = std::make_shared<spore_list_t>();
            else{
                if(_m2._p_spore_list)
                    _m._p_spore_list->assign(_m2._p_spore_list->begin(), _m2._p_spore_list->end());
                if(_m1._p_spore_list)
                    _m._p_spore_list->assign(_m1._p_spore_list->begin(), _m1._p_spore_list->end());
            }
            if(_m2._p_polymer_list || _m2._p_polymer_list)
                _m._p_polymer_list = std::make_shared<polymer_list_t>();
            else{
                if(_m2._p_polymer_list)
                    _m._p_polymer_list->assign(_m2._p_polymer_list->begin(), _m2._p_polymer_list->end());
                if(_m1._p_polymer_list)
                    _m._p_polymer_list->assign(_m1._p_polymer_list->begin(), _m1._p_polymer_list->end());
            }
            _m._p_in_neure = std::make_shared<neure>(_type, {dynamic_pointer_cast<base_object>(_m1._p_in_neure), dynamic_pointer_cast<base_object>(_m2._p_in_neure)});
            _m._p_out_neure = std::make_shared<neure>(_type, {dynamic_pointer_cast<base_object>(_m1._p_out_neure), dynamic_pointer_cast<base_object>(_m2._p_out_neure)});
            return std::move(_m);
        }

        static matrix&& _connect(const matrix &_from, const matrix &_to){
            //_m2 必须包含输入
            if(!_to._p_in_neure || !_to._p_in_neure->has_childs()){
                //MSG...
                return std::move(matrix());
            }
            if(!_from._p_out_neure || !_from._p_out_neure->has_childs())
                return _union(_from, _to, neure_type::_multiplication); 
            matrix _m;
            _connect(_m, _from._p_out_neure, _to._p_in_neure);
            if(!_m._p_spore_list && (_from._p_spore_list || _to._p_spore_list))
                _m._p_spore_list = std::make_shared<spore_list_t>();
            if(_from._p_spore_list)
                _m._p_spore_list->assign(_from._p_spore_list->begin(), _from._p_spore_list->end());
            if(_to._p_spore_list)
                _m._p_spore_list->assign(_to._p_spore_list->begin(), _to._p_spore_list->end());
            if(!_m._p_polymer_list && (_from._p_polymer_list || _to._p_polymer_list))
                _m._p_polymer_list = std::make_shared<polymer_list_t>();
            if(_from._p_polymer_list)
                _m._p_polymer_list->assign(_from._p_polymer_list->begin(), _from._p_polymer_list->end());
            if(_to._p_polymer_list)
                _m._p_polymer_list->assign(_to._p_polymer_list->begin(), _to._p_polymer_list->end());
            return std::move(_m);
        }

        static void _connect(matrix &_matrix, p_neure_t _from_neure, p_neure_t _to_neure){
            object_list_t _from_objs;
            _from_neure->walk_childs([&](const p_object_t& _obj) -> bool{
                p_neure_t _neure = dynamic_pointer_cast<neure>(_obj);
                if(_neure){
                    p_spore_t _spore = std::make_shared<spore_odd>();
                    if(!_matrix._p_spore_list)
                        _matrix._p_spore_list = std::make_shared<spore_list_t>();
                    _matrix._p_spore_list->push_back(_spore);
                    matrix _m(_spore);
                    _connect(_matrix, _neure, _m._p_in_neure);
                    _from_objs.push_back(_m._p_out_neure);
                }else if(dynamic_pointer_cast<axon>(_obj)){
                    _from_objs.push_back(_obj);
                }
            });

            object_list_t _to_objs;
            _to_neure->walk_childs([&](const p_object_t& _obj) -> bool{
                p_neure_t _neure = dynamic_pointer_cast<neure>(_obj);
                if(_neure){
                    p_spore_t _spore = std::make_shared<spore_odd>();
                    if(!_matrix._p_spore_list)
                        _matrix._p_spore_list = std::make_shared<spore_list_t>();
                    _matrix._p_spore_list->push_back(_spore);
                    matrix _m(_spore);
                    _connect(_matrix, _m._p_out_neure, _neure);
                    _to_objs.push_back(_m._p_in_neure);
                }else if(dynamic_pointer_cast<axon>(_obj)){
                    _to_objs.push_back(_obj);
                }
            });
            if(_from_objs.size() > 1 && _to_objs.size() > 1){
                p_spore_t _spore = std::make_shared<spore_odd>();
                _matrix._p_spore_list->push_back(_spore);
                matrix _m(_spore);
                _connect(_matrix, _m._p_out_neure, std::make_shared<neure>(_to_objs));
                _to_objs.clear();
                _to_objs.push_back(_m._p_in_neure);
            }else
            {

            }
        }

    protected:
        p_neure_t _p_in_neure{nullptr};
        p_neure_t _p_out_neure{nullptr};
        p_spore_list_t _p_spore_list{nullptr};
        p_polymer_list_t _p_polymer_list{nullptr};
    };

}



#endif/*flying_spore_matrix_h*/