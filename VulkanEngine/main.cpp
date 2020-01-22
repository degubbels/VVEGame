/**
* The Vienna Vulkan Engine
*
* (c) bei Helmut Hlavacs, University of Vienna
*
*
* Edit degu
*/


#include "VEInclude.h"
//#include "VEGameInclude.h"
//#include "MidiReader.h"

#include <fstream>
#include <vector>
#include <iostream>

using namespace std;



namespace ve {


	uint32_t g_score = 0;				//derzeitiger Punktestand
	double g_time = 30.0;				//zeit die noch übrig ist
	bool g_gameLost = false;			//true... das Spiel wurde verloren
	bool g_restart = false;			//true...das Spiel soll neu gestartet werden


	// Course width
	const int COURSE_WIDTH = 8;
	const float RUNNING_SPEED = 15.0f;

	// The time elapsed so far
	float time;

	
	VESceneNode *eParent;

	//
	//Zeichne das GUI
	//
	class EventListenerGUI : public VEEventListener {
	protected:
		
		/**
		*	Draw GUI overlay
		*/
		virtual void onDrawOverlay(veEvent event) {
			VESubrenderFW_Nuklear * pSubrender = (VESubrenderFW_Nuklear*)getRendererPointer()->getOverlay();
			if (pSubrender == nullptr) return;

			// Get context
			struct nk_context * ctx = pSubrender->getContext();

			// Create GUI window
			nk_begin(ctx, "", nk_rect(0, 0, 0, 0), NK_WINDOW_BORDER);

			// TODO: GUI


			//if (!g_gameLost) {
			//	if (nk_begin(ctx, "", nk_rect(0, 0, 200, 170), NK_WINDOW_BORDER )) {
			//		char outbuffer[100];
			//		nk_layout_row_dynamic(ctx, 45, 1);
			//		sprintf(outbuffer, "Score: %03d", g_score);
			//		nk_label(ctx, outbuffer, NK_TEXT_LEFT);

			//		nk_layout_row_dynamic(ctx, 45, 1);
			//		sprintf(outbuffer, "Time: %004.1lf", g_time);
			//		nk_label(ctx, outbuffer, NK_TEXT_LEFT);
			//	}
			//}
			//else {
			//	if (nk_begin(ctx, "", nk_rect(500, 500, 200, 170), NK_WINDOW_BORDER )) {
			//		nk_layout_row_dynamic(ctx, 45, 1);
			//		nk_label(ctx, "Game Over", NK_TEXT_LEFT);
			//		if (nk_button_label(ctx, "Restart")) {
			//			g_restart = true;
			//		}
			//	}

			//};

			// Always call at the end
			nk_end(ctx);
		}

	public:
		///Constructor of class EventListenerGUI
		EventListenerGUI(std::string name) : VEEventListener(name) { };

		///Destructor of class EventListenerGUI
		virtual ~EventListenerGUI() {};
	};


	static std::default_random_engine e{ 12345 };					//Für Zufallszahlen
	static std::uniform_real_distribution<> d{ -10.0f, 10.0f };		//Für Zufallszahlen

	//
	// Überprüfen, ob die Kamera die Kiste berührt
	//
	class EventListenerCollision : public VEEventListener {
	protected:
		virtual void onFrameStarted(veEvent event) {
			static uint32_t cubeid = 0;

			if (g_restart) {
				g_gameLost = false;
				g_restart = false;
				g_time = 30;
				g_score = 0;
				getSceneManagerPointer()->getSceneNode("The Cube Parent")->setPosition(glm::vec3(d(e), 1.0f, d(e)));
				getEnginePointer()->m_irrklangEngine->play2D("media/sounds/ophelia.mp3", true);
				return;
			}
			if (g_gameLost) return;

			glm::vec3 positionCube   = getSceneManagerPointer()->getSceneNode("The Cube Parent")->getPosition();
			glm::vec3 positionCamera = getSceneManagerPointer()->getSceneNode("StandardCameraParent")->getPosition();

			float distance = glm::length(positionCube - positionCamera);
			if (distance < 1) {
				g_score++;
				getEnginePointer()->m_irrklangEngine->play2D("media/sounds/explosion.wav", false);
				if (g_score % 10 == 0) {
					g_time = 30;
					getEnginePointer()->m_irrklangEngine->play2D("media/sounds/bell.wav", false);
				}

				VESceneNode *eParent = getSceneManagerPointer()->getSceneNode("The Cube Parent");
				eParent->setPosition(glm::vec3(d(e), 1.0f, d(e)));

				getSceneManagerPointer()->deleteSceneNodeAndChildren("The Cube"+ std::to_string(cubeid));
				VECHECKPOINTER(getSceneManagerPointer()->loadModel("The Cube"+ std::to_string(++cubeid)  , "media/models/test/crate0", "cube.obj", 0, eParent) );
			}

			g_time -= event.dt;
			if (g_time <= 0) {
				g_gameLost = true;
				getEnginePointer()->m_irrklangEngine->removeAllSoundSources();
				getEnginePointer()->m_irrklangEngine->play2D("media/sounds/gameover.wav", false);
			}
		};

	public:
		///Constructor of class EventListenerCollision
		EventListenerCollision(std::string name) : VEEventListener(name) { };

		///Destructor of class EventListenerCollision
		virtual ~EventListenerCollision() {};
	};

	class EventListenerFrame : public VEEventListener {
	
	protected:
		virtual void onFrameStarted(veEvent event) {


			// Auto move forward
			VECamera* pCamera = getSceneManagerPointer()->getCamera();
			VESceneNode* pParent = pCamera->getParent();

			glm::vec4 translate = glm::vec4(0.0, 0.0, 0.0, 1.0);
			translate = pCamera->getTransform() * glm::vec4(0.0, 0.0, 1.0, 1.0); //forward
			translate.y = 0.0f;

			///add the new translation vector to the previous one
			float speed = RUNNING_SPEED;
			glm::vec3 trans = speed * glm::vec3(translate.x, translate.y, translate.z);
			pParent->multiplyTransform(glm::translate(glm::mat4(1.0f), (float)event.dt * trans));

			spawnNotes(event);
		};

		virtual void spawnNotes(veEvent event) {
			float newTime = time + event.dt;

			if (newTime > 6.0f && time < 6.0f) {
				VESceneNode* e1;
				VECHECKPOINTER(e1 = getSceneManagerPointer()->loadModel("The Cube0", "media/models/test/crate0", "cube.obj"));
				eParent->multiplyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 120.0f)));
				eParent->addChild(e1);
				printf("crate");
			}


			time = newTime;

		}

	public:
		///Constructor of class EventListenerCollision
		EventListenerFrame(std::string name) : VEEventListener(name) { };

		///Destructor of class EventListenerCollision
		virtual ~EventListenerFrame() {};
	};

	///user defined manager class, derived from VEEngine
	class VEGame : public VEEngine {
	public:

		VEGame( bool debug=false) : VEEngine(debug) {};
		~VEGame() {};


		///Register an event listener to interact with the user
		
		virtual void registerEventListeners() {
			VEEngine::registerEventListeners();

			//registerEventListener(new EventListenerCollision("Collision"), { veEvent::VE_EVENT_FRAME_STARTED });
			registerEventListener(new EventListenerGUI("GUI"), { veEvent::VE_EVENT_DRAW_OVERLAY});
			registerEventListener(new EventListenerFrame("Frame"), { veEvent::VE_EVENT_FRAME_STARTED });
		};
		

		///Load the first level into the game engine
		///The engine uses Y-UP, Left-handed
		virtual void loadLevel( uint32_t numLevel=1) {

			VEEngine::loadLevel(numLevel );			//create standard cameras and lights

			VESceneNode *pScene;
			VECHECKPOINTER( pScene = getSceneManagerPointer()->createSceneNode("Level 1", getRoot()) );
	
			//scene models

			// Skybox
			VESceneNode *sp1;
			VECHECKPOINTER( sp1 = getSceneManagerPointer()->createSkybox("The Sky", "media/models/test/sky/cloudy",
										{	"bluecloud_ft.jpg", "bluecloud_bk.jpg", "bluecloud_up.jpg", 
											"bluecloud_dn.jpg", "bluecloud_rt.jpg", "bluecloud_lf.jpg" }, pScene)  );

			// Groundplane
			VESceneNode *e4;
			VECHECKPOINTER( e4 = getSceneManagerPointer()->loadModel("The Plane", "media/models/test", "plane_t_n_s.obj",0, pScene) );
			e4->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(COURSE_WIDTH, 1.0f, 10000.0f)));

			//VEEntity *pE4;
			//VECHECKPOINTER( pE4 = (VEEntity*)getSceneManagerPointer()->getSceneNode("The Plane/plane_t_n_s.obj/plane/Entity_0") );
			//pE4->setParam( glm::vec4(1000.0f, 1000.0f, 0.0f, 0.0f) );

			
			eParent = getSceneManagerPointer()->createSceneNode("The Cube Parent", pScene, glm::mat4(1.0));
			
			

			m_irrklangEngine->play2D("media/sounds/songs/Austria_anthem.midi", true);
		};
	};

	struct Note {
		int start;
		int duration;
		int note;
		int volume;
	};


	class Midi {

		struct TempoChange
		{
			int atMicro;	// Time in micros at which to change
			int newMicrosPerQuarterNote;
		};

		// Queue of notes per channel
		vector<queue<Note>> notes;

		string trackName = "Lorem Ipsum";
		int microsPerQuarterNote = 500'000;	// Default 120 bpm
		int ticksPerQuarterNote;
		int microsPerTick;

		bool isSmallEndian() {
			unsigned int i = 1;
			char* c = (char*)&i;
			if (*c)
				return true;
			else
				return false;
			return 0;
		}


		void swapByteOrder(unsigned short& us)
		{
			us = (us >> 8) |
				(us << 8);
		}

		void swapByteOrder(unsigned int& ui)
		{
			ui = (ui >> 24) |
				((ui << 8) & 0x00FF0000) |
				((ui >> 8) & 0x0000FF00) |
				(ui << 24);
		}

		void swapByteOrder(unsigned long long& ull)
		{
			ull = (ull >> 56) |
				((ull << 40) & 0x00FF000000000000) |
				((ull << 24) & 0x0000FF0000000000) |
				((ull << 8) & 0x000000FF00000000) |
				((ull >> 8) & 0x00000000FF000000) |
				((ull >> 24) & 0x0000000000FF0000) |
				((ull >> 40) & 0x000000000000FF00) |
				(ull << 56);
		}

		void correctEndian(unsigned int& ui) {
			if (isSmallEndian())
			{
				swapByteOrder(ui);
			}
		}

		void correctEndian(unsigned short& ui) {
			if (isSmallEndian())
			{
				swapByteOrder(ui);
			}
		}

	public:
		Midi() {};

		void readFile(string fileName) {

			// Only read format 1

			vector<char*> chunks;
			vector<int> chunkLengths;


			// Open file
			ifstream file(fileName, ios::in | ios::binary);

			// Read header
			byte type[4];
			file.read((char*)type, 4);
			string typestr((char*)type, 4);
			printf("type: %s\n", type);
			printf("type: %s\n", typestr.c_str());

			unsigned int headerLength;
			file.read((char*)&headerLength, 4);
			correctEndian(headerLength);
			printf("length: %i\n", headerLength);

			uint16_t format; // 0, 1, 2
			file.read((char*)&format, 2);
			correctEndian(format);
			printf("format: %i\n", format);

			if (format != 1)
			{
				// Invalid format
				printf("INVALID FORMAT, only 1 is accepted");
				return;
			}

			uint16_t tracks;
			file.read((char*)&tracks, 2);
			correctEndian(tracks);
			printf("tracks: %i\n", tracks);

			// ticks per quarter note
			uint16_t division;
			file.read((char*)&division, 2);
			correctEndian(division);
			printf("division: %i\n", division);
			ticksPerQuarterNote = division;


			// Read chunks
			byte chunkType[4];
			unsigned int chunkLength[4];
			char* chunk;

			// TODO: check that numChunks does indeed match numTracks
			for (size_t i = 0; i < tracks; i++)
			{
				file.read((char*)chunkType, 4);
				string typestr((char*)type);

				unsigned int chunkLength;
				file.read((char*)&chunkLength, 4);
				correctEndian(chunkLength);
				printf("Chunk %i: length %i\n", i, chunkLength);

				if (chunkLength < 0) {
					printf("invalid chunk length\n");
					break;
				}

				chunk = (char*)malloc(chunkLength);
				file.read(chunk, chunkLength);
				chunks.push_back(chunk);
				chunkLengths.push_back(chunkLength);
			}

			// Current time since start of song in microseconds
			int currentMicroTime;
			int currentVolume = 63;		// Volume 0-127

			// Calculate for default
			microsPerTick = (microsPerQuarterNote / ticksPerQuarterNote);

			// Changes in the tempo by point of start
			queue<TempoChange> tempoChangesProto = *(new queue<TempoChange>());;
			queue<TempoChange> tempoChanges = *(new queue<TempoChange>());

			notes.resize(16);

			// Track channel vector<queue<Note>>
			// vector of currently sounding notes per channels
			vector<vector<Note>> startedNotes(chunks.size(), vector<Note>(128));

			// Loop tracks
			for (size_t i = 0; i < chunks.size(); i++) {

				// Reset time
				currentMicroTime = 0;

				// Reset tempoChanges
				tempoChanges = tempoChangesProto;

				// Loop events in track
				bool endOfTrack = false;
				for (size_t e = 0; e < chunkLengths[i]; e++)
				{
					// chunkLengths[i] is number of bytes in chunk, not number of events


					// dtime in ticks
					unsigned int dtimeTicks;
					int vlqlen = getVariableLengthQuantityValue((char*)chunks[i], &dtimeTicks);
					//printf("l: %X, dt: %X\n", length, dtimeTicks);

					currentMicroTime += dtimeTicks * microsPerTick;

					// Check that the tempo is still the same
					while (!tempoChanges.empty() 
						&& currentMicroTime > tempoChanges.front().atMicro) {
						// New tempo
						microsPerTick = (tempoChanges.front().newMicrosPerQuarterNote / ticksPerQuarterNote);
						tempoChanges.pop();
					}
					

					// Next bytes
					chunks[i] += vlqlen;

					// Number of bytes in events
					unsigned int length;

					// Event type
					if ((byte)chunks[i][0] == 0xFF) {
						// META

						// type
						printf("metatype: %X\n", (byte)(chunks[i][1]));
						switch ((byte)chunks[i][1]) {
							case 0x03:	// Track name
							{
								length = (byte)chunks[i][2];

								/*printf("byte: %s\n", (char*)&(chunks[i][3]));*/
								string name((char*)&(chunks[i][3]), length);
								trackName = name;
								printf("name: %s\n", trackName.c_str());

								break;
							}
							case 0x51: // Tempo
							{

								length = (byte)chunks[i][2];

								byte tempoBytes[3] = { chunks[i][3], chunks[i][4], chunks[i][5] };
								uint32_t tempo = *((unsigned int*)tempoBytes);
								correctEndian(tempo);

								// Shift one byte right
								tempo = tempo >> 8;
								microsPerQuarterNote = tempo;
								printf("tempo: %X\n", (int)microsPerQuarterNote);

								// Change current tempo
								microsPerTick = (microsPerQuarterNote / ticksPerQuarterNote);

								// Record change for other tracks
								tempoChangesProto.push({ currentMicroTime, (int)tempo });

								break;
							}
							case 0x58: // Time Signature
							{
								length = (byte)chunks[i][2];

								int nn = (byte)chunks[i][3];
								int dd = (byte)chunks[i][4];
								int cc = (byte)chunks[i][5];
								int bb = (byte)chunks[i][6];

								printf("sig: (%d/%d) with cc=%d, dd=%d\n", nn, (int)pow(2,dd), cc, bb);

								// TODO: use

								break;
							}							
							case 0x59:	// Key signature
							{
								length = (byte)chunks[i][2];

								signed int sf = (byte)chunks[i][3];
								bool isMinor = (byte)chunks[i][4];
								if (isMinor) {
									printf("sf: %i, minor: true\n", sf);
								}
								else {
									printf("sf: %i, minor: false\n", sf);
								}
								
								break;
							}
							case 0x2F: // End of Track
							{
								length = (byte)chunks[i][2];

								endOfTrack = true;

								break;
							}
							default:
							{
								// Skip this message
								length = (byte)chunks[i][2];

								break;
							}
						}
						
						// Add bytes for event type, meta type, length
						length += 3;

					}
					else if ((byte)chunks[i][0] == 0xF0 ||
						(byte)chunks[i][0] == 0xF7) {
						// SysEx

						length = (byte)chunks[i][1];
						vlqlen = getVariableLengthQuantityValue(&(chunks[i][1]), &length);

						printf("sysex, len: %d\n", length);

						// Add bytes for event type, length
						length += 1 + vlqlen;


					}
					else {
						// Midi
						//printf("midi\n");

						// First four bits signify type
						byte type = 0xF0 & (byte)chunks[i][0];
						// Second four bits indicate midi channel
						byte channel = 0x0F & (byte)chunks[i][0];
						//printf("midi type: %X, channel: %X\n", type, channel);

						switch (type) {
							case 0x80:	// Note Off
							{
								length = 3;

								// Get note
								unsigned int n = (byte)chunks[i][1];

								Note note = startedNotes[channel][n];
								note.duration = currentMicroTime - note.start;

								// Put into notes on this channel
								notes[channel].push(note);
								
								break;
							}
							case 0x90:	// Note On
							{
								length = 3;

								// get note
								unsigned int n = (byte)chunks[i][1];
								printf("note %d on\n", n);
								// Velocity ignored

								// Register started note
								Note note = { currentMicroTime, 0, n, currentVolume };
								startedNotes[channel][n] = note;

								break;
							}
							case 0xA0:	// Polyphonic pressure
							{
								length = 3;

								// Not used
								break;
							}
							case 0xB0: // Controller
							{
								length = 3;

								byte controller = (byte)chunks[i][1];
								byte value = (byte)chunks[i][1];
								// Controller 7 is channel volume
								break;
							}
							case 0xc0:	// Program change
							{
								length = 2;

								// Not used
								break;
							}
							case 0xD0:	// Channel pressure
							{
								length = 2;

								// Not used
								break;
							}
							case 0xE0:	// Pitch bend
							{
								length = 3;

								// Not used;
								break;
							}
							default:
							{
								// Shouldn't occur
								printf("Impossible midi type\n");

								// Assume length 3
								length += 3;
								break;
							}								
						}

						// event type byte already included in length
					}

					chunks[i] += length;

					// End of chunk reached
					if (endOfTrack) {
						printf("End of track\n");
						break;
					}
				}

			}

			printf("End of File\n");

		}


		// Quantity of 1-4 bytes
		int getVariableLengthQuantityLength(char* q) {
			
			int i;

			// Reverse, as midi is big-endian
			for (i = 0; i < 4; i++) {

				uint8_t byt = q[i];

				// 7th not bit set -> this was the last byte
				if (byt < 0b1000'0000)
				{

					break;
				}
			}

			// To count
			i += 1;

			return i;
		}

		// Quantity of 1-4 bytes
		/**
		 * Returns length
		 */
		int getVariableLengthQuantityValue(char* q, unsigned int *out) {


			int i;
			unsigned int value = 0;

			// Reverse, as midi is big-endian
			for (i = 0; i < 4; i++) {

				value = value * 128;

				uint8_t byt = q[i];

				value += (byt & ~0b1000'0000);

				// 7th not bit set -> this was the last byte
				if (byt < 0b1000'0000)
				{

					break;
				}
			}
			
			*out = value;

			// To count
			i += 1;

			return i;
		}
	};

}

using namespace ve;


/**
 * program entrypoint
 */
int main() {
	
	string fileName = "media/sounds/songs/SuperMario64-Medley.mid";

	Midi read;
	read.readFile(fileName);

	return 0;

	bool debug = true;

	VEGame mve(debug);	//enable or disable debugging (=callback, validation layers)

	mve.initEngine();
	mve.loadLevel(1);
	mve.run();

	return 0;
}

