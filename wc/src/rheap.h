#ifndef RHEAP_INCLUDED
#define RHEAP_INCLUDED
/*
Copyright (C) 2013 Anders Musikka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <vector>
#include <stdexcept>
#include <limits.h>
#include <stdint.h>

template<typename T>
class RHeap
{
    static const int buckets=4096;

    int curheadcost; //bucket
    int farfutureheadcost;
    std::vector<T> bins[buckets];
    std::vector<T> farfuture;
    int count;

    void realize_farfuture()
    {
    	std::vector<T> newfarfuture;
    	int newfarfutureheadcost=INT_MAX;
    	int farfuturelimit=curheadcost+buckets;
    	for(const auto& t : farfuture)
    	{
    		int cost=costof(t);
    		int off=cost&(buckets-1);
    		if (cost>=farfuturelimit)
    		{
    			newfarfuture.push_back(t);
    			if (cost<newfarfutureheadcost)
    				newfarfutureheadcost=cost;
    			continue;
    		}
    		bins[off].push_back(t);
    	}
    	std::swap(farfuture,newfarfuture);
    	std::swap(newfarfutureheadcost,farfutureheadcost);

    }
public:
    RHeap()
    {
        curheadcost=0;
        farfutureheadcost=INT_MAX;
        count=0;
    }
    int size()
    {
        return count;
    }

    T pop()
    {
        if (count==0) throw std::runtime_error("Can't get top, no items!");
        for(;;)
        {
            if (curheadcost==farfutureheadcost)
            {
            	realize_farfuture();
            }
            unsigned int bucket_nr=(curheadcost)&(buckets-1);
            std::vector<T>& bucket=bins[bucket_nr];
            if (!bucket.empty())
            {
                T ret=bucket.back();
				--count,
				bucket.pop_back();
				return ret;
            }
            ++curheadcost;
        }
    }
    void push(T t)
    {
        unsigned int cost=costof(t);
    	if (count==0)
    	{
    		curheadcost=cost;
    	}

        while(curheadcost>(int)cost) //Reverse curheadcost until new value may be written
        {
        	curheadcost-=1;

        	int bucket_nr=curheadcost&(buckets-1);
        	if (!bins[bucket_nr].empty())
        	{
				farfuture.insert(farfuture.end(),bins[bucket_nr].begin(),bins[bucket_nr].end());
				int cleared_bucket_cost=curheadcost+buckets;
				if (cleared_bucket_cost<farfutureheadcost)
					farfutureheadcost=cleared_bucket_cost;

				bins[bucket_nr].clear();
        	}
        }

        unsigned int futurelimit=curheadcost+buckets;
        if (cost>=futurelimit)
        {

        	if ((int)cost<farfutureheadcost)
        		farfutureheadcost=cost;
        	farfuture.push_back(t);
            ++count;
        	return;
        }
        unsigned int bucket_nr=cost&(buckets-1);
        ++count;
        bins[bucket_nr].push_back(t);
    }

    bool empty()
    {
        return count==0;
    }


};


#endif

