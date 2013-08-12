#include "Split.h"
#include <iostream>

Split* Split::s_instance = NULL; 

int Split::push( const char* str )
{
    if( str == NULL )
    {
        return -1;
    }
    if( segCount < MAX_SEG_COUNT -1 )
    {

        segment[segCount++] = str;
        return 0;
    }
    return -1;
}

string& Split::print()
{
    out = "";
    if( segCount == 0 )
    {
        return out;
    }
    out += segment[0];
    for( unsigned int i = 1; i < segCount; ++i )
    {
        out +="\t";
        out += segment[i];
    }
    return out;
}

string& Split::print( char splitstr )
{
    out = "";
    if( segCount == 0 )
    {
        return out;
    }
    out += segment[0];
    for( unsigned int i = 1; i < segCount; ++i )
    {
        out += splitstr;
        out += segment[i];
    }
    return out;
}

void Split::split( const char* str, char* sp, unsigned int seg /*= MAX_SEG_COUNT */ )
{
    clear();
    if( str == NULL || sp == NULL )
    {
        return;
    }
    segCount = 0; 
    if (seg <= 0 )
    {
        segment[0] = str;
        ++segCount;
        return;
    }		
    int step = strlen(sp);
    const char* spos = str;
    while( 1 )
    {
        const char* p;
        p = strstr(spos, sp );
        if( p == NULL )
        {
            int len = strlen(spos);
            segment[segCount].assign(spos,len);
            ++segCount;
            break;
        }
        else
        {
            int len = p - spos;
            segment[segCount].assign(spos,len);
            spos = p + step;
            ++segCount;
        }
        if(segCount == MAX_SEG_COUNT )
        {
            break;
        }
        if( segCount == seg )
        {
            segment[segCount] = spos;
            ++segCount;
            break;
        }
    }
}

void Split::split_ex( const char* str )
{
    clear();
    if( str == NULL )
    {
        return;
    }
    segCount = 0;            
    const char* spos = str;
    bool lastIsSpace = false;
    while( *spos != 0 )
    {
        if( *spos == ' ' || *spos == '\t')
        {
            ++spos;
            if( !lastIsSpace )
            {
                ++segCount;
            }
            lastIsSpace = true;
            if(segCount == MAX_SEG_COUNT )
            {
                break;
            }				
            continue;
        }
        else
        {
            lastIsSpace = false;
            segment[segCount].push_back(*spos);
            ++spos;
        }
        if(segCount == MAX_SEG_COUNT )
        {
            break;
        }
    }
    if( *str != 0 && segCount < MAX_SEG_COUNT )
    {
        ++segCount;
    }
}

void Split::split( const char* str, char sp,unsigned int seg /*= MAX_SEG_COUNT */ )
{
    char temp[2] = "";
    temp[0] = sp;
    temp[1] = 0;
    split(str,temp,seg);
}

void Split::clear()
{
    for(unsigned int i = 0; i < segCount; ++i )
    {
        segment[i].clear();
    }
}

Split::Split()
{
    segCount = 0;
    for( int i = 0; i < MAX_SEG_COUNT; ++i )
    {
        segment[i].reserve(RESERVE_SIZE);
    }
}

Split * Split::instance()
{
    if (s_instance == NULL)
    {
        s_instance = new Split();
    }
    return s_instance;
}

Split::~Split()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }
    s_instance = NULL;
}

unsigned int Split::size()
{
    return this->segCount;
}

bool Split::set( unsigned int index , const char* str )
{
    if( str == NULL ){
        return false;
    }
    if( index < segCount )
    {
        segment[index] = str;
        return true;
    }
    return false;
}

bool Split::set( unsigned int index , double value )
{
    char temp[SMALL_BUFFER];
    sprintf(temp,"%.2f",value);
    return set(index,temp);
}

