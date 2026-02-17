
function OnCreate(self)
	self.health = 0;
	self.temp = 0;

	local velocity1 = vec3.new(0, 1, 0);
	local velocity2 = vec3.new(0, 1, 4);
	local velocitySum = velocity1 + velocity2;
end

function OnUpdate(self, dt)
	self.temp = self.temp + (0.3 * dt);
	local position = GetWorldPosition(self.Id);
	position.z = math.sin(math.rad(self.temp)); -- set current entity x-pos to temp variable in degrees

	--for i = self.temp, self.temp + 1000 do
	--	self.temp = self.temp + 1;
	--end
end

function OnDestroy(self)
	Log("Entity destroyed", 0)
end
