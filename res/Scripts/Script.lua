
function OnCreate(self)
	Log("Entity has been created", 0)
	self.health = 0
	Log(tostring(self.health), 0)
end

function OnUpdate(self, dt)

	self.health = self.health + 1
	--Log(tostring(self.health), 0)
end

function OnDestroy(self)
	Log("Entity destroyed", 0)
end
