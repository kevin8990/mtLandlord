#ifndef 	__MTFL_SERVICE_HALL_PROP_USE_H
#define 	__MTFL_SERVICE_HALL_PROP_USE_H

class 	mtFLServiceHallPropUse
{
public:
	struct DataWrite
		{
			long 							lStructBytes;			/// 包大小
			long                        	lServiceType;			/// 服务类型
			long 							plReservation[2];		/// 保留字段
			long                            lUserId;                /// 用户id
			long 							plPropsUseCount[10];	/// 道具使用情况
		};

		struct DataRead
		{
			long 							lStructBytes;			/// 包大小
			long                        	lServiceType;			/// 服务类型
			long 							plReservation[2];		/// 保留字段
			long                            lResult;                /// 使用道具结果
		};
};

#endif 	/// __MTFL_SERVICE_HALL_PROP_USE_H
