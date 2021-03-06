#include "th_trade_filter.h"
#include <boost/lexical_cast.hpp>

namespace th_trade_filter
{
	size_t th_trade_filter::id = 0;

	th_trade_filter::th_trade_filter()
	{
	}

	void th_trade_filter::set_threads_and_run()
	{
		for( size_t i = 0; i < th_trade_filter::thread_count; i++ )
		{
			th_gr.create_thread( boost::bind( &th_trade_filter::process, this ) );
			boost::this_thread::sleep( boost::posix_time::milliseconds(10) );
		}
		th_gr.join_all();
	}

	void th_trade_filter::process() 
	{
		while( true )
		{
			unsigned temp_id;
			bool if_end = false;
			{
				boost::mutex::scoped_lock lock( mtx );
				{
					if( id == file_count )
					{
						if_end = true;
						return;
					}
					temp_id = id;
					id++;
				}
			}
			if( if_end )
				return;

			std::string input, output;
			char id[4];
			sprintf( id, "%03d", temp_id );
			input = BINARY_DIR"/input_" + boost::lexical_cast<std::string>(id) + ".in";
			output = BINARY_DIR"/output_" + boost::lexical_cast<std::string>(id) + ".out";
			
			try
			{
				temp.run( input, output );
			}
			catch( std::exception& e )
			{
				boost::mutex::scoped_lock( mtx );
				{
					std::cerr << e.what() << temp_id << "\n";
				}
			}
		}
	}
}