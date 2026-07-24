-- Variables to store our Entities (C Pointers!)
player = nil

-- CreateEntity(x, y, w, h, speed, is_solid, max_frames, anim_speed, frame_w, scale, "image.png")
player = CreateEntity(400, 300, 40, 50, 300, true, 6, 0.15, 100, 2.5, "assets/Soldier_Idle.png")

CreateEntity(100, 100, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")
CreateEntity(600, 150, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")
CreateEntity(200, 400, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")
CreateEntity(450, 450, 100, 100, 0, true, 1, 0, 100, 0.1, "assets/player.png")

function Update(delta_time)
    local move_x = 0
    local move_y = 0
    local speed = 300

    local current_anim = GetEntitySprite(player)

    -- Check our Input Actions (0=Jump, 1=Left, 2=Up, 3=Down, 4=Right)
    if IsActionDown(1) then move_x = move_x - (speed * delta_time) end
    if IsActionDown(4) then move_x = move_x + (speed * delta_time) end
    if IsActionDown(2) then move_y = move_y - (speed * delta_time) end
    if IsActionDown(3) then move_y = move_y + (speed * delta_time) end

    if IsActionDown(1) or IsActionDown(2) or IsActionDown(3) or IsActionDown(4) then
        if current_anim ~= "assets/Soldier_Walk.png" then
            SetEntitySprite(player, "assets/Soldier_Walk.png", 8, 0.1, 100)
        end
    else
        if current_anim ~= "assets/Soldier_Idle.png" then
            SetEntitySprite(player, "assets/Soldier_Idle.png", 6, 0.15, 100)
        end
    end

    -- call the C collision logic
    MoveEntity(player, move_x, move_y)

    -- camera follows the player
    local px, py = GetEntityPosition(player)
    
    -- centering the screen on the player
    SetCameraPosition(px - 400 + 20, py - 300 + 25)
end
