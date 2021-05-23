#include "UserResourceParser.h"
#include <bytes.hpp>
#include <xref.hpp>
#include <ua.hpp>
#include <allins.hpp>
#include "common/public.h"

qvector<UserResourceParser::mid_BinSource> mVec_UserResource;          //用户资源
ea_t UserResourceParser::m_UserCodeStartAddr = 0;
ea_t UserResourceParser::m_UserCodeEndAddr = 0;

void UserResourceParser::InitUserCodeAddr(ea_t start, ea_t end)
{
	m_UserCodeStartAddr = start;
	m_UserCodeEndAddr = end;
}

UserResourceParser::BinType_t UserResourceParser::GetBinValueType(ea_t DataAddr)
{
	ea_t CodeAddr = get_first_dref_to(DataAddr);
	if (CodeAddr == BADADDR) {
		return UserResourceParser::e_UnknownValue;
	}

	//交叉引用必须在用户代码范围之内
	if (CodeAddr <= m_UserCodeStartAddr || CodeAddr >= m_UserCodeEndAddr) {
		return UserResourceParser::e_UnknownValue;
	}

	insn_t FirstIns;
	int FirstLen = decode_insn(&FirstIns, CodeAddr);

	if (FirstIns.itype == NN_mov) {
		if (FirstIns.ops[0].reg == 0x0) {      //mov eax,xxx
			return UserResourceParser::e_UnknownValue;
		}
		if (FirstIns.ops[0].reg == 0x3) {      //mov [ebx],xxx
			return UserResourceParser::e_ClassTable;
		}
		if (FirstIns.ops[0].reg == 0x6 && get_dword(CodeAddr + 5) == 0xABADABAD) {      //mov esi,xxx
			return UserResourceParser::e_ArrayHead;
		}
	}
	else if (FirstIns.itype == NN_add && FirstIns.ops[0].reg == 0x3) {
		return UserResourceParser::e_SwitchTable;
	}
	else if (FirstIns.itype == NN_fadd || FirstIns.itype == NN_fsub || FirstIns.itype == NN_fmul || FirstIns.itype == NN_fdiv || FirstIns.itype == NN_fcomp || FirstIns.itype == NN_fld)
	{
		return UserResourceParser::e_FloatValue;
	}
	else if (FirstIns.itype == NN_push)
	{
		return UserResourceParser::e_UnknownValue;
	}

	return UserResourceParser::e_UnknownValue;
}

bool UserResourceParser::ParseUserResource(ea_t lpStringStart, uint32 StringSize)
{
	mVec_UserResource.clear();

	qvector<unsigned char> tmpResouceBuf;
	tmpResouceBuf.resize(StringSize);
	get_bytes(&tmpResouceBuf[0], StringSize, lpStringStart);

	unsigned int index = 0;

	while (index < StringSize) {
		mid_BinSource tmpSource = {};

		tmpSource.itype = GetBinValueType(lpStringStart + index);
		tmpSource.address = lpStringStart + index;

		switch (tmpSource.itype)
		{
		case e_ArrayHead:
		{
			index += 8;    //数组头或空字节集大小应至少为8?

			//这句代码是否要加入还在测试观察阶段
			//while (!HasDataRef(lpStringStart + index)) {
			//	index++;
			//}
			continue;
		}
		case e_FloatValue:
		{
			index += 8;
			continue;
		}
		case e_ClassTable:
		case e_SwitchTable:
		{
			do
			{
				index++;
			} while (!HasDataRef(lpStringStart + index));
			continue;
		}
		default:
		{
			if (ReadUInt(&tmpResouceBuf[index]) == 0x1) {
				int size = ReadUInt(&tmpResouceBuf[index + 4]);
				if (!size) {
					tmpSource.itype = e_NullBin;
					index += 8;
				}
				else {
					tmpSource.itype = e_BinValue;
					tmpSource.data = QByteArray((char*)&tmpResouceBuf[index + 8], size);
					index += 8 + size;
					while (!HasDataRef(lpStringStart + index)) {
						index++;
					}
				}
				continue;
			}
			else {                    //任意字符串处理,这是最终的手段
				unsigned int len = qstrlen(&tmpResouceBuf[index]);
				if (!len) {
					tmpSource.itype = e_NullStr;
				}
				else {
					tmpSource.itype = e_StringValue;
					tmpSource.data = QByteArray((char*)&tmpResouceBuf[index], len);
				}
				index += len + 1;;
			}
		}
		break;
		}
		mVec_UserResource.push_back(tmpSource);
	}

	return true;
}

int UserResourceParser::MenuHandle_ShowUserResource()
{
	struct chooser_UserResource :public chooser_multi_t
	{
	protected:
		const int widths_[3] = { 10,10,50 };
		const char* header_[3] = { "Address","Type","Data" };
	public:
		chooser_UserResource(const char* title) :chooser_multi_t(0, qnumber(widths_), widths_, header_, title)
		{

		}
	private:
		~chooser_UserResource()
		{

		}
		const void* get_obj_id(size_t* len) const
		{
			*len = strlen(title);
			return title;
		}

		void idaapi get_row(qstrvec_t* cols_, int* icon_, chooser_item_attrs_t* attrs, size_t n) const
		{
			qstrvec_t& cols = *cols_;
			qvector<mid_BinSource>& vec_Bin = mVec_UserResource;

			cols[0].sprnt("%08a", vec_Bin[n].address);

			switch (vec_Bin[n].itype)
			{
			case e_BinValue:
			{
				//To do...
			}
			break;
			case e_StringValue:
			{
				acp_utf8(&cols[1], "文本型");
				qstring tmpStr = vec_Bin[n].data.toByteArray();
				tmpStr.replace("\x0D\x0A", "\r\n");
				acp_utf8(&cols[2], tmpStr.c_str());
			}
			break;
			case e_NullStr:
				acp_utf8(&cols[1], "空白文本");
				break;
			case e_NullBin:
				acp_utf8(&cols[1], "空白字节集");
				break;
			case e_ArrayHead:
				acp_utf8(&cols[1], "数组头");
				break;
			case e_FloatValue:
			{
				acp_utf8(&cols[1], "浮点数");
				double tmpFloatdata;
				get_bytes(&tmpFloatdata, sizeof(double), vec_Bin[n].address);
				cols[2].sprnt("%lf", tmpFloatdata);
			}
			break;
			case e_ClassTable:
				acp_utf8(&cols[1], "类虚表");
				break;
			case e_SwitchTable:
				acp_utf8(&cols[1], "分支表");
				break;
			default:
				break;
			}

			return;
		}
		cbres_t idaapi enter(sizevec_t* sel)
		{
			if (sel->size()) {
				jumpto(mVec_UserResource[sel->at(0)].address);
			}
			return cbres_t();
		}

		size_t idaapi get_count(void) const
		{
			return mVec_UserResource.size();
		}
	};

	qstring title = getUTF8String("用户常量资源");
	chooser_UserResource* pUserChoose = new chooser_UserResource(title.c_str());
	pUserChoose->choose();
	return 0;
}