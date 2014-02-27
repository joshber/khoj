#include <vector>

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/qtime/QuickTime.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Khoj {
}

class KhojApp : public AppNative {
public:
	void setup();

	void update();
	void draw();

	void loadMovies( const fs::path & );

	// To start, we're loading multiple movies but just showing one
	// Next step is multiple windows per
	// • cinder/samples/BasicAppMultiWindowApp
	// • https://github.com/simongeilfus/Cinder-CodeEditor/blob/master/samples/MultiWindow/src/MultiWindowApp.cpp#L37
	// • https://forum.libcinder.org/topic/multiple-windows-with-different-contents
	// Also question of multiple full-screen displays with separate GL contexts controlled from a single process
	// ci::Display class Settings::setDisplay() etc
	// glutFullScreen()
	// https://forum.libcinder.org/topic/multiple-screens

	vector<qtime::MovieGl> mMovies;
	gl::Texture mFrameTexture;

	vector<WindowRef> mWindows;
};

void KhojApp::setup() {
	// Eventually these will get delegated to a window instantiator
	setWindowSize(720, 480);
	setFullScreen(false);
	setFrameRate(60.0);

	loadMovies( getAssetPath( "movies" ) );
}

// Load movies into a vector
void KhojApp::loadMovies( const fs::path & folder ) {
	if ( ! fs::is_directory( folder ) ) {
		console() << "Hélas, " << folder << " is not a folder." << endl;
		return;
	}

	// Load the movies
	fs::directory_iterator end;

	for ( fs::directory_iterator p( folder ); p != end; ++p ) {
		qtime::MovieGl movie;

		try {
			movie = qtime::MovieGl( p->path() );
			
			movie.setLoop( true /* loop */, false /* palindrome */ );

			mMovies.push_back( movie );
			movie.play();

		}

		/*// create a texture for showing some info about the movie
		TextLayout infoText;
		infoText.clear( ColorA( 0.2f, 0.2f, 0.2f, 0.5f ) );
		infoText.setColor( Color::white() );
		infoText.addCenteredLine( moviePath.filename().string() );
		infoText.addLine( toString( mMovie->getWidth() ) + " x " + toString( mMovie->getHeight() ) + " pixels" );
		infoText.addLine( toString( mMovie->getDuration() ) + " seconds" );
		infoText.addLine( toString( mMovie->getNumFrames() ) + " frames" );
		infoText.addLine( toString( mMovie->getFramerate() ) + " fps" );
		infoText.setBorder( 4, 2 );
		mInfoTexture = gl::Texture( infoText.render( true ) );
		*/

		catch ( ... ) {
			movie.reset();
			console() << "Hélas, could not load " << p->path() << "." << endl;
		}
	}
}

void KhojApp::update() {
	// For the time being, we just show the first movie in the list
	if ( mMovies[1] ) {
		mFrameTexture = mMovies[1].getTexture();
	}
}

void KhojApp::draw() {
	gl::clear( Color( 0, 0, 0 ) );
	//gl::enableAlphaBlending();

	if ( mFrameTexture ) {
		gl::draw( mFrameTexture, Rectf( 0, 0, getWindowWidth(), getWindowHeight() ) );
	}
}

CINDER_APP_NATIVE( KhojApp, RendererGl )
