// fms_ob.h - Order book
#pragma once
#include <cassert>
#include <chrono>
#include <concepts>
#include <map>

namespace fms {

	template<class T>
	concept totally_regular = std::totally_ordered<T> and std::regular<T>;

	// Limit order.
	template<totally_regular T, totally_regular L, totally_regular A, std::regular E>
	struct limit {
		T time;   // time order first placed
		L level;  // order level
		A amount; // order amount > 0 buy, < 0 sell
		E entity; // legal entity placing the order.
	};
	
	namespace {
		constexpr limit<int, int, int, int> l[] = {
			{1, 2, 3, 4},
			{1, 2, 3, 4},
			{1, 2, 3, 4},
			{1, 2, 3, 4},
		};
		static_assert(l[0].time == 1 and l[0].entity == 4);
		// TODO: more tests
	}
	
	// Market order.
	template<totally_regular T, totally_regular A, std::regular E>
	struct market {
		T time;   // time order placed
		A amount; // order amount
		E entity; // entity placing the order.
	};

	// Use time as (hopefully) unique Id.
	// TODO: Replace by UUIDv7 (RFC 9562)?
	inline auto Id()
	{
		return std::chrono::high_resolution_clock::now();
		// TODO: keep last time and check against minimum time increment.
		// Add that times number of ties if equal.
	}

	template<totally_regular T, totally_regular L, totally_regular A, std::regular E>
	class order_book {
		using Id = std::invoke_result_t<decltype(Id)>;
		static_assert(totally_regular<Id>);
		static inline std::map<Id, limit<T,L,A,E>> ob;
	public:
		// Test if you want to avoid asserts.
		static const limit<T, L, A, E>* exists(Id id)
		{
			const auto i = ob.find(id);

			return i == ob.end() ? nullptr : &i->second;
		}

		static Id place(const limit<T,L,A,E>& lo)
		{
			auto id = Id(); // generate id
			
			assert(!exists(id));
			ob[id] = lo;

			return id;
		}
		
		static void cancel(Id id)
		{
			assert(ob.erase(id));
		}
		
		static limit<T,L,A,E> find(Id id)
		{
			const auto o = exists(id);
			assert(o);

			return *o;
		}

		// Return  (ask, bid) depth at level given time
		// TODO: one of these should be 0. How to deal with crosses?
		std::pair<A,A> depth(T time, L level) const
		{
			A a = 0; // ask depth
			A b = 0; // bid depth
			for (const auto& [_, lo] : ob) {
				if (lo.time < time and lo.level = level) {
					if (lo.amount > 0) {
						a += lo.amount;
					}
					else {
						b += lo.amount;
					}
				}
			}

			return { a,b };
		}
		
		// Execute a market order. Return filled orders.
		static void /* order_book */ execute(const market<T, A, E>& mo)
		{
			//order_book ob;

			for (const auto& [id, lo] : ob) {
				if (lo.time < mo.time) {
					if (mo.amount > 0) {
						if (lo.amount > mo.amount) {
						}
					}
					else {
					}
				}
			}

			//return ob;
		}
	};

} // namespace fms
