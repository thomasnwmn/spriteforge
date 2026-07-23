-- THIS IS OUR VERY FIRST GAME SCRIPT!
-- You can edit this file while the game is running to change the game without compiling!

-- Variables to store our Entities (C Pointers!)
player = nil

-- The setup code runs once when the file is loaded by C
print("Hello from Lua! Loading Level 1...")

-- CreateEntity(x, y, w, h, speed, is_solid, max_frames, anim_speed, frame_w, scale, "image.png")
player = CreateEntity(400, 300, 40, 50, 300, true, 6, 0.15, 100, 1.5, "assets/Soldier_Idle.png")

-- We scale these down to 0.1 so the high-resolution photo acts as a small "wall block"!
CreateEntity(100, 100, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")
CreateEntity(600, 150, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")
CreateEntity(200, 400, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")
CreateEntity(450, 450, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")

-- This function is called EVERY FRAME by our C engine!
function Update(delta_time)
    local move_x = 0
    local move_y = 0
    local speed = 300

    -- Check our Input Actions! (0=Jump, 1=Left, 2=Up, 3=Down, 4=Right)
    if IsActionDown(1) then move_x = move_x - (speed * delta_time) end
    if IsActionDown(4) then move_x = move_x + (speed * delta_time) end
    if IsActionDown(2) then move_y = move_y - (speed * delta_time) end
    if IsActionDown(3) then move_y = move_y + (speed * delta_time) end

    -- Call the C Collision logic!
    MoveEntity(player, move_x, move_y)

    -- Make the camera follow the player!
    local px, py = GetEntityPosition(player)
    
    -- We subtract 400 and 300 to center the 800x600 screen on the player!
    SetCameraPosition(px - 400 + 20, py - 300 + 25)
end
