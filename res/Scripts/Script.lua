
function OnCreate(self)
	self.health = 0;
	Log(tostring(self.Id), 0);

	local velocity1 = vec3.new(0, 1, 0);
	local velocity2 = vec3.new(0, 1, 4);
	local velocitySum = velocity1 + velocity2;

	Log(tostring(velocitySum.x), 0);
	Log(tostring(velocitySum.y), 0);
	Log(tostring(velocitySum.z), 0);
end

function OnUpdate(self, dt)

	self.health = self.health + 1;
	--Log(tostring(self.health), 0)
end

function OnDestroy(self)
	Log("Entity destroyed", 0)
end
