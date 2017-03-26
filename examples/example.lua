speechtotxt = require("speechtotxt")

speech = speechtotxt.init(nil,nil, "./order.dict")
txt = speech:inmic()
print(txt)

--close context
speech:close()
