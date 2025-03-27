// 채팅 얼리기

#pragma once
#include<iostream>
#include"httplib.h"
#include"DB_admin.hpp"
#include <mysql/jdbc.h>
#include <windows.h>


//void chat_freeze() {
//	if (status == 0) {		// 관리자(user_status=0) 이라면
//		chat();				// 일반적으로 채팅 가능
//	}
//	else {					// user_status가 0이 아닌경우는
//		MessageBox(NULL, TEXT("채팅창이 얼려진 상태입니다."), TEXT("FREEZE"), MB_ICONINFORMATION);
//	}						// FREEZE 상태 알림창 표시
//}

