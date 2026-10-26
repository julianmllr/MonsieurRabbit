#include "gui.h"
#include "game.h"
#include "input_handler.h"
#include <cppgl/src/imgui/imgui_internal.h>
#include <cppgl/src/stbi/stb_image.h>
#include <sstream>
#include <iomanip>

using namespace ImGui;

Texture2D Gui::pause_texture;

Texture2D Gui::health_heart_texture;
Texture2D Gui::health_heart_dark_texture;
Texture2D Gui::ground_texture;
Texture2D Gui::grass_half_texture;
Texture2D Gui::snow_half_texture;
Texture2D Gui::grass_block_texture;
Texture2D Gui::snow_block_texture;
Texture2D Gui::tree_green_texture;
Texture2D Gui::tree_snow_texture;
Texture2D Gui::ladder_texture;
Texture2D Gui::moving_platform_texture;
Texture2D Gui::rotating_platform_texture;
Texture2D Gui::click_platform_texture;
Texture2D Gui::pressure_plate_texture;
Texture2D Gui::door_texture;
Texture2D Gui::door_pressure_texture;
Texture2D Gui::spikes_texture;
Texture2D Gui::timed_spikes_texture;
Texture2D Gui::portal_texture;
Texture2D Gui::movable_block_texture;
Texture2D Gui::destroyable_block_texture;
Texture2D Gui::spike_texture;

Texture2D Gui::coin_texture;
Texture2D Gui::heart_texture;
Texture2D Gui::key_texture;
Texture2D Gui::shot_texture;
Texture2D Gui::jewel_texture;

Texture2D Gui::title;
Texture2D Gui::lvl_1;
Texture2D Gui::lvl_2;
Texture2D Gui::lvl_3;

Texture2D Gui::lvl_2_g;
Texture2D Gui::lvl_3_g;

Texture2D Gui::ghost_texture;
Texture2D Gui::zombie_texture;

string Gui::cur_filename;

ImVec4 beigeColor = ImVec4(0.85f, 0.75f, 0.6f, 1.0f); // Beige Farbe definieren
ImVec4 fontCol = ImVec4(0.0,0.0,0.0,1.0f);

float bigFontSize = 1.9f;

void Gui::loadGui()
{
    loadImages();
    //loadTimers();
    //loadBuildMenu();
}

void Gui::loadImages() {

    pause_texture = Texture2D("pause", "render-data/images/icons/pause.png");

    health_heart_texture = Texture2D("heart", "render-data/images/icons/heart.png");
    health_heart_dark_texture = Texture2D("heart_dark", "render-data/images/icons/heart_dark.png");
    ground_texture = Texture2D("ground_texture", "render-data/models/dirt/blockDirt.png");
    grass_half_texture = Texture2D("grass_half_texture", "render-data/models/grass/blockHalf.png");
    snow_half_texture = Texture2D ("snow_half_texture", "render-data/models/snow/blockSnowHalf.png");
    grass_block_texture = Texture2D("grass_block_texture", "render-data/models/grass/block.png");
    snow_block_texture = Texture2D("grass_block_texture", "render-data/models/snow/blockSnow.png");
    tree_green_texture = Texture2D("tree_green_texture", "render-data/models/nature/treePine.png");
    tree_snow_texture = Texture2D("tree_snow_texture", "render-data/models/nature/treePineSnow.png");
    ladder_texture = Texture2D("ladder_texture", "render-data/models/wood/ladder.png");
    moving_platform_texture = Texture2D("moving_platform_texture", "render-data/models/utilities/blockMoving-direction.png");
    rotating_platform_texture = Texture2D("rotating_platform_texture", "render-data/models/utilities/blockMoving-rotating.png");
    click_platform_texture = Texture2D("click_platform_texture", "render-data/models/utilities/blockMovingBlue.png");
    pressure_plate_texture = Texture2D("pressure_plate_texture", "render-data/models/utilities/buttonSquare.png");
    door_texture = Texture2D("door_texture", "render-data/models/stone/doorLargeClosed.png");
    door_pressure_texture = Texture2D("door_texture", "render-data/models/stone/doorLargeClosed-button.png");
    spikes_texture = Texture2D("spikes_texture", "render-data/models/traps/spikes-button.png");
    timed_spikes_texture = Texture2D("timed_spikes_texture", "render-data/models/traps/spikes-time.png");
    portal_texture = Texture2D("portal_texture", "render-data/models/stone/doorLargeOpen.png");
    movable_block_texture = Texture2D("movable_block_texture", "render-data/models/stone/crateStrong.png");
    destroyable_block_texture = Texture2D("destroyable_block_texture", "render-data/models/wood/crate.png");

    coin_texture = Texture2D("coin_texture", "render-data/models/utilities/coinGold.png");
    heart_texture = Texture2D("heart_texture", "render-data/models/utilities/heart.png");
    key_texture = Texture2D("key_texture", "render-data/models/utilities/key.png");
    shot_texture = Texture2D("shot_texture", "render-data/models/traps/bomb.png");
    jewel_texture = Texture2D("jewel_texture", "render-data/models/utilities/jewel.png");
    spike_texture = Texture2D("spike_texture", "render-data/models/traps/spikeBlock.png");

    title = Texture2D("title", "render-data/images/gui/title.png");
    lvl_1 = Texture2D("lvl_1", "render-data/images/gui/island-1.png");
    lvl_2 = Texture2D("lvl_2", "render-data/images/gui/island-2.png");
    lvl_3 = Texture2D("lvl_3", "render-data/images/gui/island-3.png");

    lvl_2_g = Texture2D("lvl_2_g", "render-data/images/gui/island-2-gray.png");
    lvl_3_g = Texture2D("lvl_3_g", "render-data/images/gui/island-3-gray.png");

    ghost_texture = Texture2D("ghost_texture", "render-data/graveyard_models/Previews/character-ghost.png");
    zombie_texture = Texture2D("zombie_texture", "render-data/graveyard_models/Previews/character-zombie.png");
    cout << "hier" << endl;
}

void Gui::drawMenu()
{
	glm::vec2 res = Context::resolution();
	glm::vec2 windowSize(200, 400);
    glm::vec2 windowSize2(100 * 8.6, 100 );
	glm::vec2 windowPos(res / 2.f - windowSize / 2.f + glm::vec2(0, 100));
	glm::vec2 buttonSize(-1.f, 50);
	int buttonOffset = 15;

    SetNextWindowSize(toImVec2(windowSize2));
    SetNextWindowPos(ImVec2(res.x / 2.95f, res.y / 2.95f));
    Begin("Title Text", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::Image((ImTextureID)size_t(title->id), ImVec2(100 * 8.6, 100 ));
    End();

	SetNextWindowSize(toImVec2(windowSize));
	SetNextWindowPos(toImVec2(windowPos));
	Begin("Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);
	SetWindowFontScale(1.8f);
    ImGui::PushStyleColor(ImGuiCol_Button, beigeColor); // Ändere die Farbe des Buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_Text, fontCol);
	PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
	PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
	if (Button("Start game", toImVec2(buttonSize))) {
        the_game->toLvL();
    }

	if (Button("Build mode", toImVec2(buttonSize)))
	{
        OpenPopup("Select Build Level");
	}

    if (BeginPopup("Select Build Level", ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
    {
        SetWindowPos(ImVec2(0.535f * res.x, 0.474 * res.y));
        if (Button("New Level")) {
            the_game->loadLevel("level-data/build_level.json");
            // find new filename
            string filename;
            int i = 0;
            bool fileExists = false;
            do
            {
                filename = "level-data/custom_levels/level_" + std::to_string(i) + ".json";
                fileExists = filesystem::exists(filename);
                i++;
            } while (fileExists);
            cur_filename = filename;
            the_game->startBuildMode();
        }
        // List all JSON files in 'level-data/game-levels' directory
        string path = "level-data/custom_levels";
        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().extension() == ".json")
            {
                string filename = entry.path().filename().string();

                // Create a button for each JSON file
                if (Button(filename.c_str()))
                {
                    cur_filename = entry.path().string();
                    // Call the import function with the selected file
                    the_game->loadLevel(entry.path().string());
                    the_game->startBuildMode();
                    // Close the popup
                    CloseCurrentPopup();
                }
            }
        }
        EndPopup();
    }

	static bool show_instructions = false;
	if (Button("Instructions", toImVec2(buttonSize)))
	{
		show_instructions = !show_instructions;
	}
	PopStyleVar(2);
    PopStyleColor(4);
	End();
	if (show_instructions)
	{
		Begin("Instructions", &show_instructions, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize);
		Text(
			"Monsieur Rabbit embarks on his epic quest to 'collect' some treasures for the next exhibition at his very own museum back in France!\r\nTry solving the puzzles in each level to get all three gems and maybe even all of the hidden coins. ;=)\r\n\r\nRotate the Camera by pressing the right mouse button and zoom in/out with the mouse wheel. \r\nMove the Character with WASD, hold and click platforms with left mouse button and shoot with shift + left mouse button\r\nIn order to place a block in Build Mode use the left mouse button, to remove it use shift + left mouse button.\r\nThe blue blocks in Build Mode indicate the path a moving object will take. Small objects might be a bit tricky to remove, but you'll get used to it. :D\r\n");
		End();
	}
}


void Gui::drawLvl()
{ 
	ImVec2 SCREEN_SIZE = ImVec2(Context::resolution().x, Context::resolution().y);
	ImVec2 SCREEN_MIDDLE = ImVec2(Context::resolution().x / 2, Context::resolution().y / 2);
	ImVec2 UNIT = ImVec2(SCREEN_SIZE.x / 20, SCREEN_SIZE.y / 20);

	SetNextWindowSize(SCREEN_SIZE);
	SetNextWindowPos(ImVec2(0,0));
	Begin("Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

	glm::vec2 res = Context::resolution();
	glm::vec2 windowSize(res.x, res.y);
	glm::vec2 windowPos(0, 0);


	//load models

	SetCursorPos(ImVec2(UNIT.x* 2.9, UNIT.y * 8.8));
    ImGui::Image((ImTextureID)size_t(lvl_1->id), ImVec2(200, 200));
    SetCursorPos(ImVec2(UNIT.x * 2.9, UNIT.y * 8.8));
	if(InvisibleButton("level 1", ImVec2(200, 200))){
		the_game->loadLevel("level-data/game_levels/level_0.json");
		the_game->startLevel(0); // start level with the right index!
	}


    SetCursorPos(ImVec2(UNIT.x* 8.98, UNIT.y * 8.8));
    if (the_game->unlocked2) ImGui::Image((ImTextureID)size_t(lvl_2->id), ImVec2(200, 200));
    else ImGui::Image((ImTextureID)size_t(lvl_2_g->id), ImVec2(200, 200));
    SetCursorPos(ImVec2(UNIT.x * 8.98, UNIT.y * 8.8));
    if(InvisibleButton("level 2", ImVec2(200, 200))){
        if(the_game->unlocked2){
            the_game->loadLevel("level-data/game_levels/level_1.json");
            the_game->startLevel(1); // start level with the right index!
        }
    }


    SetCursorPos(ImVec2(UNIT.x* 15.2, UNIT.y * 8.8));
    if (the_game->unlocked3) ImGui::Image((ImTextureID)size_t(lvl_3->id), ImVec2(200, 200));
    else ImGui::Image((ImTextureID)size_t(lvl_3_g->id), ImVec2(200, 200));
    SetCursorPos(ImVec2(UNIT.x * 15.2, UNIT.y * 8.8));
    if(InvisibleButton("level 3", ImVec2(200, 200))){
        if(the_game->unlocked3){
            the_game->loadLevel("level-data/game_levels/level_2.json");
            the_game->startLevel(2); // start level with the right index!
        }
    }


    int x = 0.185 * res.x;
    int y = 0.41 * res.y;
    SetCursorPos(ImVec2(x, y));
    if(the_game->coin_counts[0] == 0)
        Text("0 / ?");
    else
        Text((to_string(the_game->coin_counts[0]) + " / " + to_string(the_game->max_coin_counts[0])).c_str());

    SetCursorPos(ImVec2(0.49 * res.x, y));
    if(!the_game->unlocked2 || the_game->coin_counts[1] == 0)
        Text("0 / ?");
    else
        Text((to_string(the_game->coin_counts[1]) + " / " + to_string(the_game->max_coin_counts[1])).c_str());

    SetCursorPos(ImVec2(0.795 * res.x, y));
    if(!the_game->unlocked3 || the_game->coin_counts[2] == 0)
        Text("0 / ?");
    else
        Text((to_string(the_game->coin_counts[2]) + " / " + to_string(the_game->max_coin_counts[2])).c_str());

    SetCursorPos(ImVec2(20,20));
    ImGui::PushStyleColor(ImGuiCol_Button, beigeColor); // Ändere die Farbe des Buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_Text, fontCol);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
    SetWindowFontScale(bigFontSize);
    if (Button("Custom Levels"))
        OpenPopup("Select Level");

    if (BeginPopup("Select Level", ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
    {
        SetWindowPos(ImVec2(20,60));
        // List all JSON files in 'level-data/game-levels' directory
        string path = "level-data/custom_levels";
        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().extension() == ".json")
            {
                string filename = entry.path().filename().string();

                // Create a button for each JSON file
                if (Button(filename.c_str()))
                {
                    // Call the import function with the selected file
                    the_game->loadLevel(entry.path().string());
                    the_game->startCustomLevel();
                    // Close the popup
                    CloseCurrentPopup();
                }
            }
        }
        EndPopup();
    }
    SetCursorPos(ImVec2(Context::resolution().x - 100,20));

    if(Button("Back"))
        the_game->toMenu();
    PopStyleColor(4);
    PopStyleVar(2);
    End();
}


void Gui::drawGUI()
{
	glm::vec2 res = Context::resolution();

	glm::vec2 frame_offset(10, 10);	//To counter frame issues
	glm::vec2 windowSize(75, 75);
	glm::vec2 windowPos(0, res.y - windowSize.y);

    Begin("Stats", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
    SetWindowPos(ImVec2(10,10));
    //Hearts
    SetWindowSize(ImVec2(res.x / 2, 400));
    for (int i = 0; i < the_game->level->player->max_hp; ++i) {
        SetCursorPos(ImVec2(10 + i * 80, 10));
        if(i < the_game->level->player->hp)
            Image((void*)(intptr_t)health_heart_texture->id, toImVec2(glm::vec2(75, 75)), ImVec2(0, 1), ImVec2(1, 0));
        else
            Image((void*)(intptr_t)health_heart_dark_texture->id, toImVec2(glm::vec2(75, 75)), ImVec2(0, 1), ImVec2(1, 0));
    }

    for (int i = 0; i < the_game->level->shot_amount; ++i) {
        SetCursorPos(ImVec2(275 + i * 50, 5));
        Image((void*)(intptr_t)shot_texture->id, toImVec2(glm::vec2(75, 75)), ImVec2(0, 1), ImVec2(1, 0));
    }

    //Coins
    SetCursorPos(ImVec2(10, 80)); //80
    Image((void*)(intptr_t)coin_texture->id, toImVec2(glm::vec2(75, 75)), ImVec2(0, 1), ImVec2(1, 0));
    SetCursorPos(ImVec2(111, 90)); //90
    Text(to_string(the_game->level->coin_amount).c_str());
    SetCursorPos(ImVec2(158, 90)); //90
    if(!the_game->customLevel){
        if(the_game->coin_counts[the_game->level_index] != 0)
            Text(("/ " + to_string(the_game->max_coin_counts[the_game->level_index])).c_str());
        else
            Text("/ ?");
    } else {
        if(the_game->level->max_coins != 0)
            Text(("/ " + to_string(the_game->level->max_coins)).c_str());
        else
            Text("/ ?");
    }
    //Keys
    SetCursorPos(ImVec2(10, 150));
    Image((void*)(intptr_t)key_texture->id, toImVec2(glm::vec2(75, 75)), ImVec2(0, 1), ImVec2(1, 0));
    SetWindowFontScale(4.0f);
    SetCursorPos(ImVec2(111, 160));
    Text(to_string(the_game->level->key_amount).c_str());
    End();

    Begin("Pause Button", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    SetWindowPos(ImVec2(Context::resolution().x - 100,10));
    SetWindowSize(ImVec2(75, 75));
    PushStyleColor(ImGuiCol_Button, beigeColor); // Ändere die Farbe des Buttons
    PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    PushStyleColor(ImGuiCol_ButtonActive, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    if(ImageButton("Pause", (void*)(intptr_t)pause_texture->id, ImVec2(50,50), ImVec2(0, 1), ImVec2(1, 0))){
        if(the_game->state == GAME_ACTIVE)
            the_game->Pause();
        else
            the_game->Resume();
    }
    PopStyleColor(3);
    End();
}

void Gui::drawPause() {
    Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    SetWindowPos(ImVec2(Context::resolution().x - 120,73));
    SetWindowSize(ImVec2(200, 500));
    ImGui::PushStyleColor(ImGuiCol_Button, beigeColor); // Ändere die Farbe des Buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_Text, fontCol);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
    SetWindowFontScale(bigFontSize);
    if(Button("Restart", ImVec2(100,35)))
        the_game->restart();
    if (Button("Menu", ImVec2(100,35)))
        the_game->toLvL();
    PopStyleColor(4);
    PopStyleVar(2);
    End();
}

void Gui::drawBuildMode()
{
	glm::vec2 res = Context::resolution();

	ImVec2 windowSize(50, 50);
	ImVec2 windowPos(0, res.y - windowSize.y);

    Begin("Building Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    SetWindowPos(ImVec2(Context::resolution().x - 120,20));
    SetWindowSize(ImVec2(200, 500));
    ImGui::PushStyleColor(ImGuiCol_Button, beigeColor); // Ändere die Farbe des Buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    ImGui::PushStyleColor(ImGuiCol_Text, fontCol);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
    SetWindowFontScale(bigFontSize);
    if (Button("Export", ImVec2(100,35)))
    {
        // export
        the_game->level->exportToFile(cur_filename);
    }
    if (Button("Menu", ImVec2(100,35))){
        InputHandler::placer.clear();
        the_game->toMenu();
    }
    PopStyleColor(4);
    PopStyleVar(2);
    End();

	windowPos = ImVec2(0.0f, 0.0f);
	Begin("Build Mode 1", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
	SetWindowPos(windowPos);
    SetWindowSize(ImVec2(90,res.y));
    ImVec2 imageSize = ImVec2(res.y/22,res.y/22);
	// set block to be built
	if (ImageButton("Ground", (void*)(intptr_t)ground_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(GROUND);
	if (ImageButton("GrassHalf", (void*)(intptr_t)grass_half_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(GRASS_HALF);
    if (ImageButton("SnowHalf", (void*)(intptr_t)snow_half_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(SNOW_HALF);
	if (ImageButton("GrassBlock", (void*)(intptr_t)grass_block_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(GRASS_BLOCK);
    if (ImageButton("SnowBlock", (void*)(intptr_t)snow_block_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(SNOW_BLOCK);
	if (ImageButton("GrassTree", (void*)(intptr_t)tree_green_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(TREE_GREEN);
	if (ImageButton("SnowTree", (void*)(intptr_t)tree_snow_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(TREE_SNOW);
	if (ImageButton("Ladder", (void*)(intptr_t)ladder_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(LADDER);
    if (ImageButton("Platform", (void*)(intptr_t)moving_platform_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))) {
        InputHandler::placer.set(PLATFORM);
        InputHandler::placer.add(OCCUPIED);
    }
    if (ImageButton("RotatingPlatform", (void*)(intptr_t)rotating_platform_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(ROTATING_PLATFORM);
    if (ImageButton("ClickPlatform", (void*)(intptr_t)click_platform_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))) {
        InputHandler::placer.set(CLICK_PLATFORM);
        InputHandler::placer.add(OCCUPIED);
    }
//	if (ImageButton("PressurePlate", (void*)(intptr_t)pressure_plate_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))){
//        InputHandler::placer.set(SPIKES);
//        InputHandler::placer.add(PRESSURE_PLATE);
//    }
	if (ImageButton("Door", (void*)(intptr_t)door_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))){
        InputHandler::placer.set(DOOR);
    }
	if (ImageButton("DoorPressure", (void*)(intptr_t)door_pressure_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))){
        InputHandler::placer.set(DOOR);
        InputHandler::placer.add(PRESSURE_PLATE);
    }
    if (ImageButton("Spikes", (void*)(intptr_t)spikes_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))){
        InputHandler::placer.set(SPIKES);
        InputHandler::placer.add(PRESSURE_PLATE);
    }
    if (ImageButton("TimedSpikes", (void*)(intptr_t)timed_spikes_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(TIMED_SPIKES);
    if (ImageButton("Portal", (void*)(intptr_t)portal_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))){
        InputHandler::placer.set(PORTAL);
        InputHandler::placer.add(PORTAL);
    }
    if (ImageButton("Movable", (void*)(intptr_t)movable_block_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(MOVABLE_BLOCK);
    if(ImageButton("DestroyableBlock", (void*)(intptr_t)destroyable_block_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(DESTROYABLE_BLOCK);

    End();
    windowPos = ImVec2((80.0f / res.x) * res.x, 0.0f);
    Begin("Build Mode 2", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    SetWindowPos(windowPos);
    SetWindowSize(ImVec2(90,res.y));
	if (ImageButton("Coin", (void*)(intptr_t)coin_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
		InputHandler::placer.set(COIN);
	if (ImageButton("Heart", (void*)(intptr_t)heart_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
		InputHandler::placer.set(HEART);
	if (ImageButton("Key", (void*)(intptr_t)key_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
		InputHandler::placer.set(KEY);
	if (ImageButton("Shot", (void*)(intptr_t)shot_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
		InputHandler::placer.set(SHOT);
    if (ImageButton("Jewel", (void*)(intptr_t)jewel_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
        InputHandler::placer.set(STAR);

	if (ImageButton("Ghost", (void*)(intptr_t)ghost_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
		InputHandler::placer.set(GHOST);
	if (ImageButton("Zombie", (void*)(intptr_t)zombie_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0)))
		InputHandler::placer.set(ZOMBIE);
    if (ImageButton("Spike", (void*)(intptr_t)spike_texture->id, imageSize, ImVec2(0, 1), ImVec2(1, 0))) {
        InputHandler::placer.set(SPIKE);
        InputHandler::placer.add(OCCUPIED_ENEMY);
    }
	End();
}

void Gui::drawWin() {
    glm::vec2 res = Context::resolution();
    ImVec2 windowSize(res.x, res.y);
    ImVec2 windowPos(0, 0);
    Begin("Won", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    PushStyleColor(ImGuiCol_WindowBg, ImVec4(1,0,0,1));
    PushStyleColor(ImGuiCol_Button, beigeColor); // Ändere die Farbe des Buttons
    PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    PushStyleColor(ImGuiCol_ButtonActive, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    PushStyleColor(ImGuiCol_Text, beigeColor);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

    SetWindowPos(windowPos);
    SetWindowSize(windowSize);
    SetWindowFontScale(5.0f);
    const char *text = "You won :)";
    float fontSize = GetFontSize();
    ImVec2 textSize = CalcTextSize(text);
    float posX = (res.x - textSize.x) * 0.5f;
    float posY = (res.y - textSize.y - fontSize) * 0.5f;
    SetCursorPos(ImVec2(posX, posY));
    Text("%s", text);

    SetWindowFontScale(bigFontSize);
    PopStyleColor(1);
    PushStyleColor(ImGuiCol_Text, fontCol);
    const char *text2 = "Menu";
    float fontSize2 = GetFontSize();
    ImVec2 textSize2 = CalcTextSize(text2);
    float posX2 = (res.x - textSize2.x) * 0.5f;
    float posY2 = (res.y - textSize2.y - fontSize2) * 0.5f;
    SetCursorPos(ImVec2(posX2, posY2 + 100));
    if(Button(text2))
        the_game->toMenu();
    PopStyleColor(5);
    PopStyleVar(2);
    End();
}

void Gui::drawGameOver() {
    glm::vec2 res = Context::resolution();
    ImVec2 windowSize(res.x, res.y);
    ImVec2 windowPos(0, 0);
    Begin("GameOver", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    PushStyleColor(ImGuiCol_WindowBg, ImVec4(1,0,0,1));
    PushStyleColor(ImGuiCol_Button, beigeColor); // Ändere die Farbe des Buttons
    PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    PushStyleColor(ImGuiCol_ButtonActive, ImVec4(beigeColor.x + 0.1f, beigeColor.y + 0.1f, beigeColor.z + 0.1f, beigeColor.w));
    PushStyleColor(ImGuiCol_Text, beigeColor);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    PushStyleVar(ImGuiStyleVar_Alpha, 1.f);

    SetWindowPos(windowPos);
    SetWindowSize(windowSize);
    SetWindowFontScale(5.0f);
    const char *text = "You lost :(";
    float fontSize = GetFontSize();
    ImVec2 textSize = CalcTextSize(text);
    float posX = (res.x - textSize.x) * 0.5f;
    float posY = (res.y - textSize.y - fontSize) * 0.5f;
    SetCursorPos(ImVec2(posX, posY));
    Text("%s", text);

    SetWindowFontScale(bigFontSize);
    PopStyleColor(1);
    PushStyleColor(ImGuiCol_Text, fontCol);
    const char *text2 = "Restart";
    float fontSize2 = GetFontSize();
    ImVec2 textSize2 = CalcTextSize(text2);
    float posX2 = (res.x - textSize2.x) * 0.5f;
    float posY2 = (res.y - textSize2.y - fontSize2) * 0.5f;
    SetCursorPos(ImVec2(posX2, posY2 + 100));
    if(Button(text2))
        the_game->restart();

    const char *text3 = "Menu";
    float fontSize3 = GetFontSize();
    ImVec2 textSize3 = CalcTextSize(text3);
    float posX3 = (res.x - textSize3.x) * 0.5f;
    float posY3 = (res.y - textSize3.y - fontSize3) * 0.5f;
    SetCursorPos(ImVec2(posX3, posY3 + 140));
    if (Button(text3))
        the_game->toMenu();
    PopStyleColor(5); // Pop the window background color
    PopStyleVar(2);
    End();
}
