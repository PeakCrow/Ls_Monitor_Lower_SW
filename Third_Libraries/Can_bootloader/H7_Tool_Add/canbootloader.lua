-------------------------------------------------------
--   
--     H7-TOOL CAN FD脱机烧录Lua小程序实现
--
-------------------------------------------------------	
local str
local len
local bytes
local bin
local offset
local value
local count
local filesize
local byte0
local byte1
local byte2
local byte3
local filepath = "0:/H7-TOOL/Lua/CAN脱机烧录/Run_Xian.bin" -- 表示CAN脱机烧录文件夹下存的文件
local filepath1 = "0:/H7-TOOL/Lua/CAN脱机烧录"        -- 浏览CAN脱机烧录文件下存的文件
local can_id = 0x111  -- 目标板的ID
local str_offset
local str_offset1


-------------------------------------------------------
--   第1步：浏览串口脱机烧录文件夹下存的文件
-------------------------------------------------------
f_dir(filepath1)
print()

-------------------------------------------------------
--   第2步：CAN FD配置
-------------------------------------------------------
print("CAN FD测试")

--第1个参数
----open，close，send，recive
--第2个参数
----- 0 表示FDCAN_FRAME_CLASSIC
----- 1 表示FDCAN_FRAME_FD_NO_BRS
----- 2 表示FDCAN_FRAME_FD_BRS
--第3个参数，支持的数据个数
---- 表示64个字节
--第4个参数，仲裁阶段波特率
--第5个参数，数据阶段波特率
can_bus("open", 0, 8, 500000, 2000000) 

-------------------------------------------------------
--   第3步：发送固件大小，方便目标板擦除相应大小扇区
-------------------------------------------------------
-- 获取固件大小
filesize=f_size(filepath)    
print("============================================")
str= string.format("固件大小：%d",filesize)
print(str)

-- 将固件大小转换成四个字节
byte0 = ((filesize >> 0) & 0xFF)
byte1 = ((filesize >> 8) & 0xFF)
byte2 = ((filesize >> 16) & 0xFF)
byte3 = ((filesize >> 24) & 0xFF)

--发送固件大小给目标板
--发送*号表示固件大小命令
--发送固件大小
--固定发送64字节
str_offset = string.format("%02d", 64 - 5)
str= string.format("%c%c%c%c%c".."%"..str_offset.."s", 42, byte0, byte1, byte2, byte3, "A")

print(str)

--第1个参数
----open，close，send，recive
--第2个参数
----ID类型，0表示标准ID，1表示扩展ID
--第3个参数
----0数据帧，1遥控帧
--第4个参数
----字符串
can_bus("send", 0, 0, can_id, str)

print("执行扇区擦除.....")

-- 正常这里应该等待CAN返回应答0x30，暂时用延迟实现
delayms(2000) 

-------------------------------------------------------
--   第4步：发送固件大小
-------------------------------------------------------
offset = 0
-- 第1个参数是路径，第2个参数的偏移地址，第3个参数读取大小
-- 返回值bytes表示读取的字节数，bin表示都回的数据
bytes, bin = f_read(filepath, 0, 32) 
offset = offset + bytes

-- 读取数据为0，表示传输完毕
while(bytes > 0)
do
    -- 发送$表示开始传输固件命令
    -- 发送固件数据给目标板
    -- 固定每次发送64个字节
    count = 8 - 2 - bytes
    str_offset = string.format("%02d", count)
    str_offset1 = string.format("%"..str_offset.."s", "A")
    str_offset2 =  string.format("$%c", bytes)
    str= str_offset2..bin..str_offset1
    can_bus("send", 0, 0, can_id, str)
    
    -- 等待目标板返回确认数据0x30
    -- 返回0x30表示发送成功
    -- 暂时用延迟实现
    delayms(30)
    
    bytes, bin = f_read(filepath, offset, 32)  -- 继续读取数据
    offset = offset + bytes
    if(bytes ~= 0) then                         -- 读取不为0，打印发送的总字节数
        print("发送固件:", offset)
    end
end

-------------------------------------------------------
--   第5步：发送传输结束命令
-------------------------------------------------------
str_offset = string.format("%02d", 8 - 1)
str= string.format("#".."%"..str_offset.."s", "A")
can_bus("send", 0, 0, can_id, str)
print("固件传输完成")

-------------------------------------------------------
--  end of file
-------------------------------------------------------