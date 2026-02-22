
-- Delete functions you do not need
function OnCreate(self)
	Log("Hello from the Miku lua script", 0);
end

function OnUpdate(self, dt)
	local cameraFront = GetSceneCameraFrontVector();

	-- Using Action axes instead of buttons/direct peripherals
	-- This allows the engine to choose which input to choose.
	-- It handles Keyboard-Mouse, Controllers
	local walkAxes = GetWalkActionAxes();

	-- NB This math is wrong!!
	local walkingDirection = vec3.new(cameraFront.x * walkAxes.x, 0.0, cameraFront.z * walkAxes.y); --TODO: Add normalize function for vectors

	local position = GetWorldPosition(self.Id);
	SetWorldPosition(self.Id, position + walkingDirection);
end

function OnDestroy(self)
	Log("Entity destroyed", 0);
end
