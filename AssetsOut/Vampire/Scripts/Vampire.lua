outputDebugString("Executing Vampire.lua")
function runScript(args)
    local pos = args['base']['pos']
    local u = args['base']['u']
    local v = args['base']['v']
    local n = args['base']['n']
    evt = root.CharacterControl.Events.Event_CreateVampire.Construct(l_getGameContext(),
        args['skinName'], args['skinPackage'], 
        pos[1], pos[2], pos[3],
        u[1], u[2], u[3],
        v[1], v[2], v[3],
        n[1], n[2], n[3],
        args['peuuid'],
    )
    handler = getGameObjectManagerHandle(l_getGameContext())
    root.PE.Components.Component.SendEventToHandle(handler, evt)
end
