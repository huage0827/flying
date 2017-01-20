/*!
 * \file core.h
 * \date 2016/12/06 18:05
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * 本文档是"flying spore" 原型设计草案的一部分，它描述了数据层的结构定义。主要考虑的是如何提供一个工整的结构来支持函数式编程的API。
 *
 * \note
*/

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED
#include <memory>

namespace flyingspore
{
    namespace meta{
        	class _pack_t;
	class _meta_cell;
	class _i_rawdata_builder;

	typedef long long _id_t;
	typedef unsigned char *_p_rawdata_t;
    typedef unsigned char *_data_t;

	typedef bool (*_pfn_pack_to_left_neure)(const _pack_t&&);
	typedef bool (*_pfn_pack_to_right_neure)(const _pack_t&&, const _meta_cell* const );

	struct _meta_id{
		_id_t id;
	};

	//data description meta
	struct _meta_data_description: public _meta_id{

	};


	//raw data meta 
	struct _meta_raw_data : public _meta_id{
		~_meta_raw_data(){
			//根据_alloc_id和_pdata来释放数据
		}
		_p_rawdata_t* _pdata;
		_id_t _alloc_id; //数据分配池的ID，每个cell分配的数据，ID为cell的ID，否则为预定义的分配池ID 
	};
	typedef std::shared_ptr<_meta_raw_data> _p_meta_raw_data_t;


	//spore meta data struct
	struct _meta_spore : public _meta_id{

	};

	//cell meta data struct
	struct _meta_cell : public _meta_id{


	};

	//axon meta data struct
	enum _meta_axon_type{UNKNOWN = 0, IN_AXON = 0xFFFF0000, OUT_AXON = 0x0000FFFF};
	struct _meta_axon : public _meta_id{
		_id_t _cell_id;
		_meta_axon_type _type;
		_id_t _Te_id;
	};

	//Te meta data struct
	/*
	Te负责桥接Pin和Cell，当一个Cell里有多个子Cell时，且父Cell的某个输入型Pin需要转发包给多个子Cell的输入型Pin时，
	Te就负责转发的逻辑。对于输出型的Pin转发时也是同样道理。这样，至少在数据汇合和分流时可以做统一操作。
	*/
	struct _meta_neure : public _meta_id{
		_i_rawdata_builder* _rawdata_builder;
		_pfn_pack_to_left_neure _to_in_neure;
		_pfn_pack_to_right_neure _to_out_neure;
	};
    typedef std::shared_ptr<_meta_neure> _p_meta_neure_t;

	//path meta data struct
	struct _meta_path : public _meta_id{
		_id_t _data_description_id;
		_id_t _left_axon_id;
		_id_t _right_axon_id;
	};

	//pack meta data struct
	struct _meta_pack : public _meta_id{
		_data_t _data; //
		_id_t _path_id;
	};
	typedef std::unique_ptr<_meta_pack> _p_meta_pack_t;

    /*---------------------------------------------------------------------------------*/

    }

}





#endif // CORE_H_INCLUDED