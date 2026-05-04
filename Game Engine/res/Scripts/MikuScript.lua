
-- Delete functions you do not need
function OnCreate(self)
	Log("Hello from the Miku lua script", 0);
end

function OnUpdate(self, dt)
	local cameraFront = GetSceneCameraFrontVector();
	local cameraRight = GetSceneCameraRightVector();

	-- Using Action axes instead of buttons/direct peripherals
	-- This allows the engine to choose which input to use.
	-- It handles Keyboard-Mouse, Controllers
	local walkAxes = GetWalkActionAxes();

	local walkingDirectionY = vec3.new(cameraFront.x, 0.0, cameraFront.z) * walkAxes.y; --TODO: Add normalize function for vectors
	local walkingDirectionX = vec3.new(cameraRight.x, 0.0, cameraRight.z) * walkAxes.x; --TODO: Add normalize function for vectors
	local walkingDirection = walkingDirectionX + walkingDirectionY;
	local walkingSpeed = 0.02;

	local position = GetWorldPosition(self.Id);
	SetWorldPosition(self.Id, position + (walkingDirection * walkingSpeed * dt));
end

function OnDestroy(self)
	Log("Entity destroyed", 0);
end
