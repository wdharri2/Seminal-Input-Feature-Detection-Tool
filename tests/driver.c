#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CMD 10
#define INITIAL_CAPACTIY 10

#define INITIAL_READ_CAPACITY 5
#define RESIZE_FACTOR 2


typedef struct VTypeStruct {
    void (*print)( struct VTypeStruct const *v );
    bool (*equals)( struct VTypeStruct const *a, struct VTypeStruct const *b );
    unsigned int (*hash)( struct VTypeStruct const *b );
    void (*destroy)( struct VTypeStruct *v );
} VType;

typedef struct NodeStruct {
    VType *key;
    VType *value;
    struct NodeStruct *next;
} Node;

struct MapStruct {
    Node **table;
    int tlen;
    int size;
};

typedef struct MapStruct Map;

typedef struct {
    void (*print)( struct VTypeStruct const *v );
    bool (*equals)( struct VTypeStruct const *a, struct VTypeStruct const *b );
    unsigned int (*hash)( struct VTypeStruct const *b );
    void (*destroy)( struct VTypeStruct *v );
    char *str;
    int length;
} Text;

typedef struct {
  void (*print)( struct VTypeStruct const *v );
  bool (*equals)( struct VTypeStruct const *a, struct VTypeStruct const *b );
  unsigned int (*hash)( struct VTypeStruct const *b );
  void (*destroy)( struct VTypeStruct *v );
  int val;
} Integer;


char *readLine(FILE *fp)
{
    int length = 0;
    int size = INITIAL_READ_CAPACITY;
    char *s = (char *) malloc( INITIAL_READ_CAPACITY * sizeof( char ) );
    
    int ch = ' ';
    
    while ( (ch = fgetc( fp )) && ch != '\n' && ch != EOF ) {
 
        s[ length ] = ch;
        length++;
        
        if ( length == size ) {
            size *= RESIZE_FACTOR;
            s = (char *) realloc( s, size * sizeof( char *));
        }
    }
    
    if ( ch == EOF && length == 0 ) {
        free( s );
        return NULL;
    }
    
    s[ length ] = '\0';
    return s;
}

Map *makeMap( int len )
{
    Map *m = (Map *) malloc( sizeof( Map ) );
    m -> size = 0;

    m -> table = (Node **) malloc( len * sizeof( Node *));
    for ( int i = 0; i < len; i++ )
        (m -> table)[ i ] = NULL;
    
    m -> size = 0;
    m -> tlen = len;
  
    return m;
}

int mapSize( Map *m )
{
    return m -> size;
}

void mapAdd( Map *m, VType *key, VType *val )
{
    unsigned int hash = key -> hash( key );
    int index = hash % (*m).tlen;
    
    Node *node = (Node *) malloc(sizeof( Node ));
    node -> key = key;
    node -> value = val;
    node -> next = NULL;
    
    if ( !( m -> table )[ index ])
        ( m -> table)[ index ] = node; 
    else {
        Node *current = ( m -> table )[ index ];
        
        while ( current -> next )
            current = current -> next;
        current -> next = node;
    }
    m -> size++;
}

void mapResize( Map *m )
{
    Node **oldTable = m -> table;
    int oldLength = m -> tlen;
    
    m -> tlen *= RESIZE_FACTOR;
    m -> table = ( Node ** ) malloc( m -> tlen * sizeof( Node *));
    m -> size = 0;
    
    for ( int i = 0; i < m -> tlen; i++ )
        (m -> table)[ i ] = NULL;
    
    for ( int i = 0; i < oldLength; i++ ) {
        Node *current = oldTable[ i ];
        
        while ( current ) {
            Node *oldNode = current;
            mapAdd( m, current -> key, current -> value );
            current = current -> next;
            free( oldNode );
        }
    }
    free( oldTable );
}

void mapSet( Map *m, VType *key, VType *val )
{
    unsigned int hash = key -> hash( key );
    int index = hash % (*m).tlen;
    
    Node *node = (Node *) malloc( sizeof( Node ));
    node -> key = key;
    node -> value = val;
    node -> next = NULL;
        
    if ( !( m -> table )[ index ])
        ( m -> table)[ index ] = node; 
    else {
        Node *current = ( m -> table )[ index ];
        
        while ( current ) {
            
            if ( current -> key -> equals( current -> key, key) ) {
                key -> destroy( key );
                current -> value -> destroy( current -> value );
                free( node );
                current -> value = val;
                return;
            }
            
            if ( !(current -> next) )
                break;
            current = current -> next;
        }

        current -> next = node;
    }
    (*m).size ++;
    if ( (*m).size == (*m).tlen )
        mapResize( m );
}

VType *mapGet( Map *m, VType *key )
{
    unsigned int hash = key -> hash( key );
    int index = hash % (*m).tlen;
    
    if ( !( m -> table )[ index ] )
        return NULL;
    else {
        Node *current = ( m -> table )[ index ];
        
        while ( current ) {
            if ( current -> key -> equals( current -> key, key ))
                return current -> value;
            
            current = current -> next;
        }
    }
    return NULL;
}

void freeNode( Node *node )
{
    node -> key -> destroy( node -> key );
    node -> value -> destroy( node -> value );
                
    free( node );
}

bool mapRemove( Map *m, VType *key )
{
    unsigned int hash = key -> hash( key );
    int index = hash % (*m).tlen;
    
    if ( !( m -> table)[ index ] )
        return false;
    else {
        Node *current = ( m -> table )[ index ];
        
        if ( current -> key -> equals( current -> key, key )) {
        
            Node *removed = current;
            m -> table[ index ] = removed -> next;
            freeNode( removed );
            
            (*m).size --;
            return true;
        }
        
        while ( current -> next ) {
            if ( current -> next -> key -> equals( current -> next -> key, key) ) {

                Node *removed = current -> next;
                current -> next = removed -> next;
                freeNode( removed );
                
                (*m).size --;
                return true;
            }
            
            current = current -> next;
        }
    }
    return false;
}

void freeMap( Map *m )
{
    for ( int i = 0; i < (*m).tlen; i++ ) {
    
        Node *removed = ( m -> table )[ i ];
        while ( removed ) {
            Node *current = removed -> next;
            freeNode( removed );
            removed = current;
        }
    }
    free( m -> table );
    free( m );
}

static void printVType( struct VTypeStruct const *v )
{
    Text const *this = (Text const *) v;
    
    bool escape = false;
    int length = strlen( this -> str );
    
    for ( int i = 0; i < length; i++ ) {
    
        if ( this -> str[ i ] == '\\' && !escape) 
            escape = true;
        else if ( escape ) {
            switch( this -> str[ i ] ) {
                case 'n':
                    printf("\n");
                    break;
                case 't':
                    printf("\t");
                    break;
                case '\\':
                    printf("\\");
                    break;
                case '\"':
                    printf("\"");
                    break;
                default:
                    printf("\%c", this -> str[ i ]);
                    break;
            }
            escape = false;
        }
        else 
            printf("%c", this -> str[ i ]);
    }
}

static bool equalsVType( struct VTypeStruct const *a, struct VTypeStruct const *b )
{
    if ( b -> print != printVType )
        return false;

    Text const *this = ( Text const *) a;
    Text const *other = ( Text const *) b;

    return strcmp( this -> str, other -> str) == 0;
}

static unsigned int hashVType( struct VTypeStruct const *b )
{
    Text const *this = ( Text const *) b;
    int i = 1;
    unsigned int hash = 0;
    
    char *value = this -> str;
    
    while ( i != this -> length - 1) {
        hash += value [ i++ ];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

static void destroyVType( struct VTypeStruct *v )
{
    Text const *this = ( Text const *) v;
    free( this -> str );
    free( v );
}

VType *parseText( char const *init, int *n )
{
    int len = 0;
    int count = 0;
    int size = INITIAL_CAPACTIY;
    char ch = '\0';
        
    if ( sscanf( init, "%c", &ch) != 1 )
        return NULL;
    
    Text *this = (Text *) malloc( sizeof( Text ) );
    this -> str = (char *) malloc( INITIAL_CAPACTIY * sizeof(char));
    
    bool quote = false;
    bool escape = false;
    
    while ( sscanf( init + len, "%c", &ch) == 1 && ch != '\0') {
    
        if ( count + 1 == size ) {
            size *= RESIZE_FACTOR;
            this -> str = (char *) realloc( this -> str, size * sizeof( char *));
        }
    
        if ( ch == '\\' )
            escape = true;
        if ( !escape && ch == '\"' && quote ) {
            this -> str [ count++ ] = ch;
            len++;
            break;
        }
        else if ( ch == '\"' && !quote )
            quote = true;
        
        if ( quote )
            this -> str [ count++ ] = ch;
        len++;
    }
    
    this -> str[ count ] = '\0';
    
    if ( n )
        *n = len;
  
    this -> length = count;
    this -> print = printVType;
    this -> equals = equalsVType;
    this -> hash = hashVType;
    this -> destroy = destroyVType;

    return (VType *) this;
}


static void print( VType const *v )
{
    Integer const *this = (Integer const *) v;
    printf( "%d", this->val );
}

static bool equals( VType const *a, VType const *b )
{
    if ( b->print != print )
        return false;

    Integer const *this = (Integer const *) a;
    Integer const *that = (Integer const *) b;

    return this->val == that->val;
}

static unsigned int hash( VType const *v )
{
    Integer const *this = (Integer const *) v;
    return this->val;
}

static void destroy( VType *v )
{
    free( v );
}


VType *parseInteger( char const *init, int *n )
{
    int val, len;
    if ( sscanf( init, "%d%n", &val, &len ) != 1 ) {
        return NULL;
    }

    if ( n )
        *n = len;
  
    Integer *this = (Integer *) malloc( sizeof( Integer ) );
    this->val = val;
    this->print = print;
    this->equals = equals;
    this->hash = hash;
    this->destroy = destroy;

    return (VType *) this;
}

static VType *parseVType( char const *init, int *n )
{
  VType *val = parseInteger( init, n );

  if ( ! val )
    val = parseText( init, n );
  
  return val;
}

static bool blankString( char *str )
{
  while ( isspace( *str ) )
    ++str;

  if ( *str )
    return false;
  return true;
}

int main()
{
  Map *map = makeMap( INITIAL_CAPACTIY );

  char *line;
  printf( "cmd> " );
  while ( ( line = readLine( stdin ) ) ) {
  
    printf( "%s\n", line );

    bool valid = false;
    char cmd[ MAX_CMD + 1 ];
    int n;
    if ( sscanf( line, "%10s%n", cmd, &n ) == 1 ) {
      char *pos = line + n;
      if ( strcmp( cmd, "get" ) == 0 ) {
        VType *k = parseVType( pos, &n );
        if ( k ) {
          pos += n;

          if ( blankString( pos ) ) {
            valid = true;
            VType *v = mapGet( map, k );
            if ( v ) {
              v->print( v );
              printf( "\n" );
            } else
              printf( "Undefined\n" );
          }

          k->destroy( k );
        }
      } else if ( strcmp( cmd, "set") == 0 ) {
        VType *k = parseVType( pos, &n );
        
        if ( k ) {
            pos += n;
            VType *v = parseVType( pos, &n );
            
            if ( v ) {
                pos += n;
                valid = true;
                mapSet( map, k, v );
            }
        }
      } else if ( strcmp( cmd, "remove") == 0 ) {
      
        VType *k = parseVType( pos, &n );
        if ( k ) {
          pos += n;

          if ( blankString( pos ) ) {
            valid = true;
            
            if ( !mapRemove( map, k ) )
                printf("Not in map\n");
          }
          k->destroy( k );
        }
      } else if ( strcmp( cmd, "size" ) == 0 ) {
        if ( blankString( pos ) ) {
          valid = true;
          printf( "%d\n", mapSize( map ) );
        }
      } else if ( strcmp( cmd, "quit" ) == 0 ) {
        free( line );
        freeMap( map );
        exit( EXIT_SUCCESS );
      }
    }

    if ( ! valid )
      printf( "Invalid command\n" );

    free( line );
    printf( "\ncmd> " );
  }

  freeMap( map );
  return EXIT_SUCCESS;
}
