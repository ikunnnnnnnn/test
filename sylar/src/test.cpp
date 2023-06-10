#include<iostream>
#include<string>
#include<vector>
using namespace  std;



class Allocator {
private:
    int m_n;
    vector<int> m_vec;
    
    int set(int start,int size,int mID){
        for(int i = 0; i < size; ++i){
            m_vec[i + start] = mID;
        }
        return start;
    }
    
public:
    Allocator(int n) : m_n(n){
        m_vec = vector<int>(m_n,0);
    }
    
    int allocate(int size, int mID) {
        if(m_n < size) return -1;
        
        int start = 0;
        int idx = 0;
        while(start + size < m_n){
            while(idx < m_n && m_vec[idx] == 0 && idx - start + 1 < size){
                idx++;
            }

            if(idx - start + 1 == size){
                int ans = set(start,size,mID);
                return ans;
            }
            
            while(idx < m_n && m_vec[idx] != 0){
                idx++;
            }
            start = idx;
        }
        return -1;
    }
    
    int free(int mID) {
        int cnt = 0;
        for(int i = 0; i < m_n; ++i){
            if(m_vec[i] == mID){
                cnt++;
                m_vec[i] = 0;
            }
        }
        return cnt;
    }
};


int main(){
    vector<int> vec;
    Allocator a(10);
    vec.emplace_back(a.allocate(1,1));
     vec.emplace_back(a.allocate(1,2));
     vec.emplace_back(a.allocate(1,3));
     a.free(2);
     vec.emplace_back(a.allocate(3,4));
     vec.emplace_back(a.allocate(1,1));
    vec.emplace_back( a.allocate(1,1));
    a.free(1);
    vec.emplace_back( a.allocate(10,2));
    a.free(7);

    for(auto v : vec){
        cout<<v<<endl;
    }
    return 0;
};