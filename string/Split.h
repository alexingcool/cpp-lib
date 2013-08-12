#ifndef _split_h_
#define _split_h_
/**
* 基于string的split类
* hehaitao, 09-03-01
*/

#include <string>
using namespace std;

/**
 *将字符串split，保存为多个段
 */
class Split
{
public:
    enum
    {
        MAX_SEG_COUNT = 128,
        RESERVE_SIZE = 40,
        SMALL_BUFFER = 64
    };
    /**
     * 大小
     */
    unsigned int size();
private:    
    string out;///<等待注释
    unsigned int segCount;///<等待注释
    string segment[MAX_SEG_COUNT];///<等待注释
    static Split * s_instance;///<等待注释
public:
    static Split * instance();
     /**
      *追加一个字段
      */
    int push( const char* str ) ;
    /**
     *设定指定位置的字符串
     */
    bool set( unsigned int index , const char* str );
    /**
     *设定指定位置的字符串
     */
    bool set( unsigned int index , double value );
    /**
     *把所有字段合成一个字符串
     */
    string& print();
    /**
     *把所有字段合成一个字符串,指定分隔符
     */
    string& print(char splitstr);
    /**
     *split字符串
     */
    void split(const char* str, char sp,unsigned int seg = MAX_SEG_COUNT );
    /**
     *split字符串
     */
    void split(const char* str, char* sp,unsigned int seg = MAX_SEG_COUNT );
    /**
     * 用非tab,空格分割
     */
    void split_ex(const char* str); 
    /**
     * clear
     */
    void clear();
    /**
     * get
     */
    const string & get(unsigned int elem) const
    {
        unsigned int pos = elem > segCount - 1 ? segCount - 1 : elem;
        return segment[pos]; 
    }
    /**
     * get
     */
    inline const string& operator[](unsigned int elem) const
    {
        return get(elem);
    }
    Split();
    virtual ~Split();
};

/**
 * 将字符串split，保存为多个段
 */
class FastSplit
{
public:
    enum
    {
        MAX_SEG_COUNT = 128
    };
    int segCount;
    char* segment[MAX_SEG_COUNT];
private:    
    string out;
public:
    /**
     *把所有字段合成一个字符串
     */
    string& print()
    {
        out = "";
        if( segCount == 0 )
        {
            return out;
        }
        int i;
        for( i = 0; i < segCount - 1; ++i )
        {
            out += segment[i];
            out +="\t";
        }    
        out += segment[i];
        return out;
    }
    /**
     *split字符串
     */
    void split( char* str, char* sp, int seg = MAX_SEG_COUNT )
    {
        clear();
        if( str == NULL || sp == NULL ){
            return;
        }

        segment[0] = str;
        segCount = 1; 
        if( seg <= 0 ){
            return;
        }
        int step = strlen(sp);
        char* spos = str;
        while( 1 )
        {
            char* p;
            p = strstr(spos, sp );
            if( p != NULL )
            {
                *p = 0;
                spos = p + step;
                segment[segCount] = spos;
                ++segCount;
            }
            else{
                break;
            }
            if(segCount == MAX_SEG_COUNT || segCount > seg ){
                break;
            }
        }
    }
    /**
     *用非tab,空格分割
     */
    void split( char* str )  // 
    {
        clear();
        if( str == NULL || *str == 0 ){
            return;
        }
        segCount = 0;            
        char* spos = str;
        bool lastIsSpace = false;
        segment[segCount] = spos;

        while( *spos != 0 )
        {
            if( *spos == ' ' || *spos == '\t'){
                *spos = 0;
                if( !lastIsSpace ){
                    ++segCount;
                    segment[segCount] = spos;
                }
                ++spos;
                lastIsSpace = true;
                if(segCount == MAX_SEG_COUNT ){
                    break;
                }                    
                continue;
            }
            else{
                if( lastIsSpace ){
                    segment[segCount] = spos;
                }
                lastIsSpace = false;
                ++spos;
            }
            if(segCount == MAX_SEG_COUNT ){
                break;
            }
        }
        if(segCount < MAX_SEG_COUNT ){
            ++segCount;
            }
    }
    void clear()
    {
        for( int i = 0; i < segCount; ++i ){
            segment[i] = NULL;
        }
    }
    inline char* operator[](int elem) const{
        int pos = elem < 0 ? 0 : elem;
        pos = elem > segCount - 1 ? segCount - 1 : elem;
        return segment[pos];
    }
    FastSplit()
    {
        segCount = 0;
        for( int i = 0; i < MAX_SEG_COUNT; ++i ){
            segment[i] = NULL;
        }
    }
};


#endif

