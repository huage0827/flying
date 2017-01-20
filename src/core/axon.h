#ifndef flying_spore_axon_h
#define flying_spore_axon_h
#include <type_traits>
#include <mutex>
#include "core.h"
#include "fobject.h"
#include "matrix.h"
#include "chain.h"
#include "data_pack_builder.h"
#include "actuator.h"
#include "data_format.h"


namespace flyingspore{
    using namespace meta;


    p_matrix_t operator & (const p_axon_t &_a1, const p_axon_t &_a2);
    p_matrix_t operator | (const p_axon_t &_a1, const p_axon_t &_a2);
    p_chain_t operator >> (const p_axon_t &_a1, const p_axon_t &_a2);
    p_chain_t operator << (const p_axon_t &_a1, const p_axon_t &_a2);

    class axon;
    typedef std::shared_ptr<axon> p_axon_t;

    class axon :fobject{
    public:
        enum  signal{data_in};

        data_pack_builder&& get_data_pack_builder();

        void push(data_pack&& _data_pack);
        void push(const data_pack& _data_pack);
        _meta_axon_type get_type() const;
        p_data_format_t get_data_format() const;
        p_actuator_t get_actuator() const;

        //matrix&& connect(p_axon_t _other){
        //    return matrix();
        //    //if(_type != ~_other->_type)
        //    //{
        //    //    //MSG... ERROR
        //    //    return std::move(matrix());
        //    //}
        //    //return std::move(matrix({std::make_shared<chain>(shared_ptr<axon>(this) , _other)}));
        //}


        //
        //matrix&& operator <<(p_axon_t _other){
        //    return matrix();
        //    //return std::move(matrix(std::shared_ptr<axon>(this)) << matrix(_other));
        //}

        //matrix&& operator>>(p_axon_t _other){
        //    return matrix();
        //    //return std::move(matrix(shared_ptr<axon>(this)) >> matrix(_other));
        //}


    protected:
        friend class cluster;
        friend class spore;
        friend class axon_builder;

        _meta_axon_type _type{UNKNOWN};
        p_data_format_t _data_format{nullptr};
        p_actuator_t _actuator{nullptr};
    };
}



#endif/*flying_spore_axon_h*/
