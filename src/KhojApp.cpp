#include <vector>

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/qtime/QuickTime.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Khoj {
	// (copied from SensorRelay.cpp)
	//
	// Keep it simple
    // The projection reaches out to meet you ... but nervously
    // If you approach too quickly it'll jump back
    // Need to send frontmost and palms:
    // {"frontmost":[tipPosition,tipVelocity,touchDistance],"palms":[[position,normal,velocity]{,2}]}
    //
    // Also, projection needs to express a tactus
    // say subtle sinusoidal fluctuation in alpha, oscillating in period and amplitude with light Gaussian noise
    // plus a pulsed stronger fluctuation in alpha,
    // the period of which oscillates slowly with Gaussian noise
    // and both periods respond to presence of visitors--gets faster, stronger gain
    // If multiple projections, alpha fluctuation should be independent ... or again, partly entrained?
    // And these oscillators need to show hysteresis in how they respond to the presence of visitors--
    // i.e., speed up and get stronger quickly as visitors arrive (maybe tailing off, so further visitors have less effect)
    // and then slowing down more slowly as the room empties ... like one's heart after a run
    // Maybe using ambient noise as the input, or one of them?
    // Maybe changing the frame rate along with alpha?
    //
    // Sensors need to countersunk in the projection surface
    // ... at different heights, for children ?
    // Pub-Sub with forwarder: http://learning-0mq-with-pyzmq.readthedocs.org/en/latest/pyzmq/devices/forwarder.html
    // JSON for msg fmt?
    // or Ggl protocol buffers? https://developers.google.com/protocol-buffers/docs/overview
    // or Thrift? or Avro?
}

class KhojApp : public AppNative {
public:
	void setup();

	void update();
	void draw();

	void loadMovies( const fs::path & );

	KhojApp() : mTotalWidth(0) {}

	// Next step is multiple windows per
	// • cinder/samples/BasicAppMultiWindowApp
	// • https://github.com/simongeilfus/Cinder-CodeEditor/blob/master/samples/MultiWindow/src/MultiWindowApp.cpp#L37
	// • https://forum.libcinder.org/topic/multiple-windows-with-different-contents
	// Also question of multiple full-screen displays with separate GL contexts controlled from a single process
	// ci::Display class Settings::setDisplay() etc
	// glutFullScreen()
	// https://forum.libcinder.org/topic/multiple-screens

	vector<qtime::MovieGl> mMovies;
	vector<gl::Texture> mFrames;
	int mTotalWidth;

	vector<WindowRef> mWindows;
};

void KhojApp::setup() {
	// FIXME Load parameters from a configuration file?

	setWindowSize(1920, 1080);
	setFullScreen(false);
	setFrameRate(60.0f);

	loadMovies( getAssetPath( "movies" ) );
}

// Load movies into a vector
void KhojApp::loadMovies( const fs::path & folder ) {
	if ( ! fs::is_directory( folder ) ) {
		console() << "Hej, " << folder << " is not a folder." << endl;
		return;
	}

	fs::directory_iterator end; // default constructor => end-of-iterator

	// Load the movies
	for ( fs::directory_iterator p( folder ); p != end; ++p ) {
		qtime::MovieGl movie;

		try {
			movie = qtime::MovieGl( p->path() );
			
			mTotalWidth += movie.getWidth();
			mMovies.push_back( movie );
			
			movie.setLoop( true /* loop */, false /* palindrome */ );
			movie.play();
		}
		catch ( ... ) {
			movie.reset();
			console() << "Hélas, could not load " << p->path() << "." << endl;
		}
	}
	mFrames.resize( mMovies.size() ); // n textures must == n movies
}

void KhojApp::update() {
	for ( int i = mMovies.size() - 1 ; i >= 0 ; --i ) {
		qtime::MovieGl movie = mMovies[i];
		if ( movie ) {
			mFrames[i] = movie.getTexture();
		}
	}
}

void KhojApp::draw() {
	gl::clear( Color( 0, 0, 0 ) );
	//gl::enableAlphaBlending();

	// FIXME Variable frame rate between movies?

	int xOffset = 0;
	for ( auto& frame : mFrames ) {
		if ( ! frame ) continue;

		int width = getWindowWidth() * frame.getWidth() / mTotalWidth;
		int height = width / frame.getAspectRatio();

		gl::draw( frame, Rectf( xOffset, 0 /* No y-offset */, xOffset + width, height ) );

		xOffset += width;
	}
}

CINDER_APP_NATIVE( KhojApp, RendererGl )
