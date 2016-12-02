#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED
#include <memory>

namespace sf
{

	class _pack_t;
	class _meta_cell;
	class _i_rawdata_builder;

	typedef long long _id_t;
	typedef unsigned char _p_rawdata_t;

	typedef bool (*_pfn_pack_to_left_te)(const _pack_t&&);
	typedef bool (*_pfn_pack_to_right_te)(const _pack_t&&, const _meta_cell* const );

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
	typedef std::shared_ptr<_meta_raw_data> _data_t;


	//spore meta data struct
	struct _meta_spore : public _meta_id{

	};

	//cell meta data struct
	struct _meta_cell : public _meta_id{


	};

	//axon meta data struct
	enum _meta_axon_type{IN, OUT};
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
	struct _meta_te : public _meta_id{
		_i_rawdata_builder* _rawdata_builder;
		_pfn_pack_to_left_te _to_left_te;
		_pfn_pack_to_right_te _to_right_te;
	};

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
	typedef std::unique_ptr<_meta_pack> _pack_t;


}





#endif // CORE_H_INCLUDED