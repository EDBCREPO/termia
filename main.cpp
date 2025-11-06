#include <nodepp/nodepp.h>
#include <nodepp/worker.h>
#include <nodepp/https.h>
#include <nodepp/json.h>
#include <nodepp/path.h>
#include <nodepp/fs.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

array_t<type::pair<bool,string_t>> memory ;
event_t<string_t> /*------------*/ onGPT  ;

/*────────────────────────────────────────────────────────────────────────────*/

object_t gpt_parts( string_t message ) {
    
    auto out = array_t<object_t>();

    memory.push({ 0, message });
    if( memory.size () > 1 )
      { memory.shift(); }

    for( auto x: memory ){ out.push( object_t({
        { "role" , string_t( x.first ? "model" : "user" ) },
        { "parts", array_t<object_t>({ object_t({ 
        { "text" , regex::replace_all( x.second, "\n", "\\n" ) }
        }) }) }
    }) );}

    return object_t({ { "contents" , out } });

}

/*────────────────────────────────────────────────────────────────────────────*/

void gpt_next( string_t message ) {
    
    fetch_t args; ssl_t ssl;
    
    args.method  = "POST";
    args.url     = process::env::get( "URL" );
    args.headers = header_t({
        { "x-goog-api-key", process::env::get("API") },
        { "Content-Type"  , "application/json" }
    });

    args.body = json::stringify( gpt_parts( message ) );

    https::fetch( args, &ssl )

    .then([=]( https_t cli ){ try {
        auto raw  = stream::await( cli );
        auto data = json::parse  ( raw );
        auto msg  = data["candidates"][0]["content"]["parts"][0]["text"].as<string_t>();
        /*memory.push({ 1, msg }); */ onGPT.emit( msg );
    } catch  ( except_t err ){ onGPT.emit( "something went wrong" ); } })

    .fail([=]( except_t err ){ onGPT.emit( "something went wrong" ); });

}

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    process::env::init( ".env" );
    auto fin = fs::std_input();

    onGPT.on([=]( string_t data ){

        auto pos = type::bind( (ulong)0 );

        process::add( coroutine::add( COROUTINE(){
        coBegin

            while( *pos < data.size() ){
                conio::log( data[*pos] );
                *pos += 1; coDelay(10);
            }

            console::log( "\n---\n" );
            conio  ::log( "->" );

        coFinish
        }));

    });

    fin.onData([=]( string_t data ){
        gpt_next( data );
    });
    
    #if _OS_ == NODEPP_OS_WINDOWS
    generator::stream::pipe task;
    worker::add( task, fin );
    #else 
    stream::pipe( fin );
	#endif
    
    gpt_next("hello!");

}

/*────────────────────────────────────────────────────────────────────────────*/
