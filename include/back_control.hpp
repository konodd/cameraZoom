#pragma once
#include <dynamixel_sdk.h>
#include "xl320_param.h"

class controlToolBox
{
public:
	controlToolBox();
	~controlToolBox();
	bool init();
	void setBaudRate(int8_t id, int8_t value);           //���ò����ʣ�0��ʾ9600bps��1��ʾ57600bps��2��ʾ115200bps��3��ʾ1Mbps
	void setOperatingMode(int8_t id, int8_t value);      //���ò���ģʽ��0��ʾ�ٶȣ�1��ʾ�Ƕ�
	void setTorqueEnable(int8_t id, int8_t value);       //����ת���Ƿ���ã�0��ʾ�����ã�1��ʾ����
	void setLED(int8_t id, int8_t value);                //����LED�ƣ�0�����ã�1�죬2�̣�3�ƣ�4����5�ۣ�6���̣�7��
	void setGoalPosition(int8_t id, int16_t value);      //����ȫ��λ��
	void setGoalVelocity(int8_t id, int16_t value);      //����ȫ���ٶ�
	void setGoalTorque(int8_t id, int16_t value);        //����ȫ��ת��
	int getPresentPosition(int8_t id);                   //��ȡ��ǰλ��
	int getPresentVelocity(int8_t id);                   //��ȡ��ǰ�ٶ�
	int getPresentLoad(int8_t id);
	bool isMoving(int8_t id);                            //�ж϶���Ƿ���ת��

private:
	dynamixel::PortHandler *portHandler;
	dynamixel::PacketHandler *packetHandler;
	dynamixel::GroupBulkWrite *bulkWrite;
	dynamixel::GroupBulkRead *bulkRead;
	int8_t dxl_comm_result;
	bool dxl_addparam_result;
	bool dxl_getdata_result;
};

controlToolBox::controlToolBox() {}

controlToolBox::~controlToolBox() {}

bool controlToolBox::init()
{
	portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);
	packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
	bulkWrite = new dynamixel::GroupBulkWrite(portHandler, packetHandler);
	bulkRead = new dynamixel::GroupBulkRead(portHandler, packetHandler);
	dxl_comm_result = COMM_TX_FAIL;
	//�򿪶˿�
	if (portHandler->openPort())
	{
		printf("Port is opened!\n");
	}
	else
	{
		printf("Port open failed!!!\n");
		return false;
	}
	//���ö˿ڲ����ʣ�������һ�����ܼ�⵽
	if (portHandler->setBaudRate(BAUDRATE))
	{
		printf("Succeeded to change port baudrate!\n");
	}
	else
	{
		printf("Failed to change port baudrate!\n");
		return false;
	}
}

void controlToolBox::setBaudRate(int8_t id, int8_t value)
{
	dxl_addparam_result = bulkWrite->addParam(id, ADDR_BAUD_RATE, LEN_BAUD_RATE, (uint8_t *)&value);
	if (dxl_addparam_result)
	{
		bulkWrite->txPacket();
		bulkWrite->clearParam();
	}
	else
	{
		printf("set baudrate failed!!!\n");
		return;
	}
}

void controlToolBox::setOperatingMode(int8_t id, int8_t value)
{
	dxl_addparam_result = bulkWrite->addParam(id, ADDR_OPERATING_MODE, LEN_OPERATING_MODE, (uint8_t *)&value);
	if (dxl_addparam_result)
	{
		bulkWrite->txPacket();
		bulkWrite->clearParam();
	}
	else
	{
		printf("set operating mode failed!!!\n");
		return;
	}
}

void controlToolBox::setTorqueEnable(int8_t id, int8_t value)
{
	dxl_addparam_result = bulkWrite->addParam(id, ADDR_TORQUE_ENABLE, LEN_TORQUE_ENABLE, (uint8_t *)&value);
	if (dxl_addparam_result)
	{
		bulkWrite->txPacket();
		bulkWrite->clearParam();
	}
	else
	{
		printf("set torqueenable failed!!!\n");
		return;
	}
}

void controlToolBox::setLED(int8_t id, int8_t value)
{
	dxl_addparam_result = bulkWrite->addParam(id, ADDR_LED, LEN_LED, (uint8_t *)&value);
	if (dxl_addparam_result)
	{
		bulkWrite->txPacket();
		bulkWrite->clearParam();
	}
	else
	{
		printf("set LED failed!!!\n");
		return;
	}
}

void controlToolBox::setGoalPosition(int8_t id, int16_t value)
{
	/*if (value > MAXPOSITION || value < MINPOSITION)
	{
	printf("Invalid positon value!!!\n");
	return;
	}*/

	if (id == 1)
	{
		if (value > DXL1_MAXPOSITION || value < DXL1_MINPOSITION)
		{
			printf("DXL%d Invalid positon value: %d\n", id, value);
			return;
		}
	}
	else if (id == 2)
	{
		if (value > DXL2_MAXPOSITION || value < DXL2_MINPOSITION)
		{
			printf("DXL%d Invalid positon value: %d\n", id, value);
			return;
		}
	}

	dxl_addparam_result = bulkWrite->addParam(id, ADDR_GOAL_POSITION, LEN_GOAL_POSITION, (uint8_t *)&value);
	if (dxl_addparam_result)
	{
		bulkWrite->txPacket();
		bulkWrite->clearParam();
	}
	else
	{
		printf("set goal position failed!!!\n");
		return;
	}
}

void controlToolBox::setGoalVelocity(int8_t id, int16_t value)
{
	dxl_addparam_result = bulkWrite->addParam(id, ADDR_GOAL_VELOCITY, LEN_GOAL_VELOCITY, (uint8_t *)&value);
	if (dxl_addparam_result)
	{
		bulkWrite->txPacket();
		bulkWrite->clearParam();
	}
	else
	{
		printf("set goal velocity failed!!!\n");
		return;
	}
}

void controlToolBox::setGoalTorque(int8_t id, int16_t value)
{
	dxl_addparam_result = bulkWrite->addParam(id, ADDR_GOAL_TORQUE, LEN_GOAL_TORQUE, (uint8_t *)&value);
	if (dxl_addparam_result)
	{
		bulkWrite->txPacket();
		bulkWrite->clearParam();
	}
	else
	{
		printf("set goal torque failed!!!\n");
		return;
	}
}

int controlToolBox::getPresentPosition(int8_t id)
{
	bulkRead->clearParam();
	dxl_addparam_result = bulkRead->addParam(id, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);

	if (dxl_addparam_result)
	{
		bulkRead->txRxPacket();
		return bulkRead->getData(id, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);
	}
	else
	{
		printf("add present position param failed!!!\n");
	}
	return -1;
}

int controlToolBox::getPresentVelocity(int8_t id)
{
	bulkRead->clearParam();
	dxl_addparam_result = bulkRead->addParam(id, ADDR_PRESENT_VELOCITY, LEN_PRESENT_VELOCITY);

	if (dxl_addparam_result)
	{
		bulkRead->txRxPacket();
		return bulkRead->getData(id, ADDR_PRESENT_VELOCITY, LEN_PRESENT_VELOCITY);
	}
	else
	{
		printf("add present velocity param failed!!!\n");
	}
	return -1;
}

int controlToolBox::getPresentLoad(int8_t id)
{
	bulkRead->clearParam();
	dxl_addparam_result = bulkRead->addParam(id, ADDR_PRESENT_LOAD, LEN_PRESENT_LOAD);

	if (dxl_addparam_result)
	{
		bulkRead->txRxPacket();
		return bulkRead->getData(id, ADDR_PRESENT_LOAD, LEN_PRESENT_LOAD);
	}
	else
	{
		printf("add present load param failed!!!\n");
	}
	return -1;
}

bool controlToolBox::isMoving(int8_t id)
{
	bulkRead->clearParam();
	dxl_addparam_result = bulkRead->addParam(id, ADDR_MOVING, LEN_MOVING);

	if (dxl_addparam_result)
	{
		bulkRead->txRxPacket();
		return bulkRead->getData(id, ADDR_MOVING, LEN_MOVING);
	}
	else
	{
		printf("add moving param failed!!!\n");
	}
	return false;
}