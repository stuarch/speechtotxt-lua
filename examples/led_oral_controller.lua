speechtotxt = require("speechtotxt")

speech = speechtotxt.init(nil,nil, "./order.dict")
txt = speech:mic()

if txt:find("on")~=nil then
    --put your turn on command here
    --os.execute("gpio write 7 1")
    print("LED on")
elseif txt:find("off")~=nil then
    --put your turn off command here
    --os.execute("gpio write 7 0")
    print("LED off")
end

speech:close()
print("\nMessage:"..txt)
