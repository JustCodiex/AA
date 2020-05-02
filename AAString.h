#pragma once
#include "AAVM.h"

class AAString : public AAObject {

public:

	AAString(std::wstring ws);

	void Release() override;

	std::wstring ToString() const override;

	size_t Length() const;

private:

	size_t m_length;
	wchar_t* m_content;

};

void AAString_Ctor(AAVM* pAAVm, any_stack& stack);
void AAString_Length(AAVM* pAAVm, any_stack& stack);
void AAString_Concat(AAVM* pAAVm, any_stack& stack);
