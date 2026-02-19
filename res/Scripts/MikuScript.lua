
-- Delete functions you do not need
function OnCreate(self)
	Log("Hello from the core lua script", 0);
end

function OnUpdate(self, dt)
	self.temp = self.temp + (0.3 * dt);
	local position = GetWorldPosition(self.Id);
	position.z = math.sin(math.rad(self.temp)); -- set current entity x-pos to temp variable in degrees
end

function OnDestroy(self)
	Log("Entity destroyed", 0);
end
