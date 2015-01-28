#pragma once

#include <boost/mpl/set_c.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/set/set0_c.hpp>

namespace static_containers
{

	namespace details
	{
		template<typename ValueType>
		bool _lookup_stage2(ValueType val1, ValueType val2)
		{
			return (val1 == val2);
		}

		template<typename ValueType, typename First, typename ... Rest>
		bool _lookup_stage2(ValueType i, First first, Rest ... rest)
		{
			if (i == first)
			{
				return true;
			}
			else
			{
				return _lookup_stage2(i, rest...);
			}
		}

		template <typename ValueType, ValueType First, ValueType ... Rest > 
		struct _static_set 
		{
			typedef boost::mpl::fold<
				boost::mpl::set_c<typename ValueType, First, Rest...>,
				boost::mpl::set0_c<typename ValueType>,
				boost::mpl::insert<boost::mpl::_1, boost::mpl::_2 >> unique_check;
			BOOST_MPL_ASSERT_RELATION(boost::mpl::size<unique_check::type>::value, == , (sizeof...(Rest)+1));
		};

		template <typename ValueType, ValueType First, ValueType ... Rest>
		bool _lookup_stage1(ValueType i, _static_set<ValueType, First, Rest...>)
		{
			return _lookup_stage2(i, First, Rest...);
		}
	}

	//+++++++++++++++

	template <typename ValueType, ValueType First, ValueType ... Rest > 
		using static_set = details::_static_set<ValueType, First, Rest...>;

	template <typename StaticSet, typename ValueType>
	bool lookup(ValueType i)
	{
		return details::_lookup_stage1(i, StaticSet());
	}

	//----------------
}