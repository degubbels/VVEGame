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

	struct Note {
		int start;
		int duration;
		int note;
		int volume;
	};

	string song = "mario";

	// Queue of notes per channel
	vector<queue<Note>> notes;

	// Original
	vector<queue<Note>> notesProto;

	// The time elapsed so far (in microseconds)
	int time;

	bool g_gameRunning = false;
	bool g_gameLost = false;			//true... das Spiel wurde verloren
	bool g_gameStart = false;
	int health = 100;

	const int MAX_HEALTH = 100;
	const int HIT_DAMAGE = 20;			// The amount of health you lose when you hit a note

	VEMesh* pMesh;
	VEMaterial* pMat;

	// Course width
	const float COURSE_WIDTH = 14.0;
	const float RUNNING_SPEED = 10.0f;
	const float NOTE_Z_OFFSET = 12.0f;
	const float NOTE_Y_OFFSET = -1.0f;
	const int TIME_OFFSET = 0;//-300'000;
	const int SPEED_CORRECTION = 1040;		// 1000 = no correction

	// Player width for collision detection purposes
	const float PLAYER_SIZE = 0.15f;
	const float PLAYER_HEIGHT = 0.7f;
	const float PLAYER_HEIGHT_OFFSET = -0.3;
	
	const float NOTE_SIZE = 0.7f;

	const float DOWN_MOVEMENT_QUOTIENT = 1.0f;
	
	VESceneNode *notesParent;

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


			if (g_gameRunning) {

				if (nk_begin(ctx, "", nk_rect(0, 0, 200, 100), NK_WINDOW_BORDER)) {
					char outbuffer[100];

					nk_layout_row_dynamic(ctx, 45, 1);
					sprintf(outbuffer, "Health: %03d", health);
					nk_label(ctx, outbuffer, NK_TEXT_LEFT);

				}
			} else {
				
				if (g_gameLost) {

					if (nk_begin(ctx, "", nk_rect(0, 0, 200, 200), NK_WINDOW_BORDER)) {
						char outbuffer[100];

						nk_layout_row_dynamic(ctx, 45, 1);
						nk_label(ctx, "Game Over", NK_TEXT_LEFT);

						
						if (nk_button_label(ctx, "Restart")) {
							g_gameStart = true;
						}
					}

				} else {

					if (nk_begin(ctx, "", nk_rect(0, 0, 200, 100), NK_WINDOW_BORDER)) {
						char outbuffer[100];

						nk_layout_row_dynamic(ctx, 45, 1);
						if (nk_button_label(ctx, "Start")) {
							g_gameStart = true;
						}
					}
				}
			}

			

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

			if (g_gameLost) return;

			glm::vec3 camPos = getSceneManagerPointer()->getSceneNode("StandardCameraParent")->getPosition();

			for (size_t i = 0; i < notesParent->getChildrenList().size(); i++) {

				ve::VESceneNode *noteNode = notesParent->getChildrenList().at(i);

				// Get world pos somehow
				glm::mat4 noteTransform = noteNode->getWorldTransform();
				glm::vec3 notePos = glm::vec3(noteTransform[3].x, noteTransform[3].y, noteTransform[3].z);

				// Get lenght encoded in name
				// Far from elegant, but works
				string lengthStr = noteNode->getName();
				lengthStr = lengthStr.substr(0, lengthStr.find_first_of("#"));
				double noteLength = stod(lengthStr);
				

				// Detect collision
				if ( abs(camPos.x - notePos.x) < (0.5*NOTE_SIZE + PLAYER_SIZE) ) {	// X-axis collide

					if ( abs((camPos.y + PLAYER_HEIGHT_OFFSET) - notePos.y) < (0.5*NOTE_SIZE + PLAYER_HEIGHT) ) {	// Y-axis collide

						if ( abs(camPos.z - notePos.z) < (0.5*noteLength + PLAYER_SIZE) ) {		// Z-axis collide

							getEnginePointer()->m_irrklangEngine->setSoundVolume(0.2);
							getEnginePointer()->m_irrklangEngine->play2D("media/sounds/explosion.wav", false);

							health -= HIT_DAMAGE;
							
							notesParent->removeChild(noteNode);
							getSceneManagerPointer()->deleteSceneNodeAndChildren(noteNode->getName());
						}
					}
				}
				
				if ( notePos.y < -5.0 ) {
					notesParent->removeChild(noteNode);
					getSceneManagerPointer()->deleteSceneNodeAndChildren(noteNode->getName());
				}
			}

			//glm::vec3 positionCube   = getSceneManagerPointer()->getSceneNode("The notes Parent")->getPosition();
			//glm::vec3 positionCamera = getSceneManagerPointer()->getSceneNode("StandardCameraParent")->getPosition();

			//float distance = glm::length(positionCube - positionCamera);
			//if (distance < 1) {
			//	//g_score++;
			//	getEnginePointer()->m_irrklangEngine->play2D("media/sounds/explosion.wav", false);
			//	//if (g_score % 10 == 0) {
			//	//	//g_time = 30;
			//	//	getEnginePointer()->m_irrklangEngine->play2D("media/sounds/bell.wav", false);
			//	//}

			//	VESceneNode *eParent = getSceneManagerPointer()->getSceneNode("The Cube Parent");
			//	eParent->setPosition(glm::vec3(d(e), 1.0f, d(e)));

			//	getSceneManagerPointer()->deleteSceneNodeAndChildren("The Cube"+ std::to_string(cubeid));
			//	VECHECKPOINTER(getSceneManagerPointer()->loadModel("The Cube"+ std::to_string(++cubeid)  , "media/models/test/crate0", "cube.obj", 0, eParent) );
			//}

			//if (g_time <= 0) {
			//	g_gameLost = true;
			//	getEnginePointer()->m_irrklangEngine->removeAllSoundSources();
			//	getEnginePointer()->m_irrklangEngine->play2D("media/sounds/gameover.wav", false);
			//}
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


			VESceneManager *sceneManager = getSceneManagerPointer();
			if (g_gameStart) {
				startGame(event);
				g_gameStart = false;
			}

			if (g_gameRunning) {
				int newTime = time + (event.dt * 1'000'000);

				// Move notes backward
				float speed = RUNNING_SPEED;
				glm::vec3 trans = speed * glm::vec3(0, -DOWN_MOVEMENT_QUOTIENT, -1.0);
				notesParent->multiplyTransform(glm::translate(glm::mat4(1.0f), (float)event.dt * trans));

				spawnNotes(event);

				if (health <= 0) {
					stopGame();
				}

				time = newTime;
			}
		};

		void stopGame() {
			g_gameLost = true;
			g_gameRunning = false;
			getEnginePointer()->m_irrklangEngine->stopAllSounds();
		}

		void startGame(veEvent event) {

			// Clean up last run
			if (g_gameLost) {
				//for (size_t i = 0; i < notesParent->getChildrenList().size(); i++) {
				//	VESceneNode *note = notesParent->getChildrenList().at(i);

				vector<VESceneNode*> noteList = notesParent->getChildrenCopy();

				for (size_t i = 0; i < noteList.size(); i++) {
					VESceneNode *note = noteList.at(i);
					getSceneManagerPointer()->deleteSceneNodeAndChildren(note->getName());
				}

				//while (notesParent->getChildrenList().size() > 0) {
				//	VESceneNode* note = notesParent->getChildrenList().back();
				//	//notesParent->removeChild(note);
				//	getSceneManagerPointer()->deleteSceneNodeAndChildren(note->getName());
				//}




				notesParent->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

				g_gameLost = false;
			}



			g_gameRunning = true;
			time = 0;

			// Deep copy
			//notes.resize(16);
			//for (size_t i = 0; i < 16; i++)
			//{
			//	notes[i] = notesProto[i];
			//}

			notes = notesProto;

			health = MAX_HEALTH;
			getEnginePointer()->m_irrklangEngine->setSoundVolume(1.0);
			getEnginePointer()->m_irrklangEngine->play2D(("media/sounds/songs/" + song + ".wav").c_str(), false);
		}

		virtual void spawnNotes(veEvent event) {
			
			// Check all midi channels
			for (size_t i = 0; i < 16; i++) {
				
				while (!notes[i].empty()
					&& time > (notes[i].front().start) + TIME_OFFSET) {

					Note note = notes[i].front();
					notes[i].pop();

					// middle C = 60
					float x = (float)(note.note - 60) / 4.0f;

					double distance = (((double)note.start / (double)1'000'000) * RUNNING_SPEED) + NOTE_Z_OFFSET;
					double length = (((double)note.duration / (double)1'000'000) * RUNNING_SPEED);
					distance += (length - 1) / 2;

					string cubeName = "cube_channel_" + to_string(i) + "_note_" + to_string(note.note) + "_at_" + to_string(note.start);

					// Encode length in name
					string nodeName = to_string(length) + "#note_node_channel_" + to_string(i) + "_note_" + to_string(note.note) + "_at_" + to_string(note.start);

					
					//VECHECKPOINTER(e1 = getSceneManagerPointer()->loadModel(cubeName, "media/models/test/crate0", "cube.obj"));
					

					VESceneNode* noteNode = getSceneManagerPointer()->createSceneNode(nodeName, notesParent, glm::mat4(1.0));
					noteNode->multiplyTransform(glm::scale(glm::mat4(1.0f), glm::vec3(NOTE_SIZE, NOTE_SIZE, length)));
					noteNode->multiplyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(x, NOTE_Y_OFFSET + DOWN_MOVEMENT_QUOTIENT*distance, distance)));					

					VESceneNode* e1;
					VECHECKPOINTER(e1 = getSceneManagerPointer()->createEntity(cubeName, pMesh, pMat, noteNode));
				}
			}

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

			registerEventListener(new EventListenerCollision("Collision"), { veEvent::VE_EVENT_FRAME_STARTED });
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

			
			vector<vh::vhVertex> vertices;

			// Skybox
			VESceneNode *sp1;
			VECHECKPOINTER( sp1 = getSceneManagerPointer()->createSkybox("The Sky", "media/models/test/sky/cloudy",
										{	"bluecloud_ft.jpg", "bluecloud_bk.jpg", "bluecloud_up.jpg", 
											"bluecloud_dn.jpg", "bluecloud_rt.jpg", "bluecloud_lf.jpg" }, pScene)  );

			// Groundplane
			VESceneNode *e4;
			VECHECKPOINTER( e4 = getSceneManagerPointer()->loadModel("The Plane", "media/models/test", "plane_t_n_s.obj",0, pScene) );
			e4->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(COURSE_WIDTH, 1.0f, 10000.0f)));

			
			notesParent = getSceneManagerPointer()->createSceneNode("The notes Parent", pScene, glm::mat4(1.0));
			//
			vector<VEMesh*> meshes;
			vector<VEMaterial*> mats;

			getSceneManagerPointer()->loadAssets("media/models/test/crate0/", "cube.obj", NULL, meshes, mats);

			pMesh = meshes[0];
			pMat = mats[0];
			
		};
	};


	class Midi {

		struct TempoChange
		{
			int atMicro;	// Time in micros at which to change
			int newMicrosPerQuarterNote;
		};

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

			notesProto.resize(16);

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

					// Check that we dont pass a tempo change
					int projectedNewMicroTime = currentMicroTime + dtimeTicks * microsPerTick;

					while (!tempoChanges.empty() 
						&& projectedNewMicroTime >= tempoChanges.front().atMicro) {
						// We would skip past a tempo change

						int microsToTempoChange = tempoChanges.front().atMicro - currentMicroTime;
						int ticksToTempoChange = microsToTempoChange / microsPerTick;

						printf("New tempo for t=%d at t=%d\n", tempoChanges.front().atMicro, currentMicroTime + microsToTempoChange);
						// New tempo
						microsPerTick = (tempoChanges.front().newMicrosPerQuarterNote / ticksPerQuarterNote);
						tempoChanges.pop();

						dtimeTicks -= ticksToTempoChange;
					}


					currentMicroTime += dtimeTicks * microsPerTick;

					// Check that the tempo is still the same
					//while (!tempoChanges.empty() 
					//	&& currentMicroTime >= tempoChanges.front().atMicro) {
					//	printf("New tempo for t=%d at t=%d\n", tempoChanges.front().atMicro, currentMicroTime);
					//	// New tempo
					//	microsPerTick = (tempoChanges.front().newMicrosPerQuarterNote / ticksPerQuarterNote);
					//	tempoChanges.pop();
					//	
					//}
					

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

								// Multiply tempo (seems to be necessary, but tempochanges still fuck up
								// Possibly different tempos per channel..
								tempo *= (1000.0/SPEED_CORRECTION);

								microsPerQuarterNote = tempo;
								//printf("tempo: %X\n", (int)microsPerQuarterNote);

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

						//length = (byte)chunks[i][1];
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
								notesProto[channel].push(note);
								
								break;
							}
							case 0x90:	// Note On
							{
								length = 3;

								// get note
								unsigned int n = (byte)chunks[i][1];
								
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
								byte value = (byte)chunks[i][2];

								if ((int)controller == 7) {
									// Controller 7 is channel volume

									printf("volume: %d\n", (int)value);
									currentVolume = (int)value;
								}
								
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
								printf("Impossible midi type: %X\n", type);

								throw ERROR_ABANDONED_WAIT_0;

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
	
	string fileName = "media/sounds/songs/"+song+".mid";



	bool debug = true;

	VEGame mve(debug);	//enable or disable debugging (=callback, validation layers)

	Midi read;
	read.readFile(fileName);

	mve.initEngine();
	mve.loadLevel(1);
	mve.run();

	return 0;
}

