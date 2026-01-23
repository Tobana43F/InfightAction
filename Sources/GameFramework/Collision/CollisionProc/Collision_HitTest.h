#pragma once

#include	<unordered_map>
#include	<vector>
#include	"Collision_Proc.h"

namespace NCollision {

	//!	@brief		UŒ‚‚Æ”íUŒ‚‚Ì“–‚½‚è”»’è‚ğs‚¤ˆ—ƒNƒ‰ƒX
	//!	@details	UŒ‚‘¤‚©‚ç‚ÌUŒ‚î•ñ‚ğæ“¾‚µA”íUŒ‚‘¤‚Éó‚¯‚½UŒ‚‚ğ“o˜^‚·‚éˆ—‚ğs‚¤
	//!	@note		—p“r
	//!				- ƒvƒŒƒCƒ„[‚ÌUŒ‚‚Æ“G‚Ì”íUŒ‚”»’è
	//!				- “G‚ÌUŒ‚‚ÆƒvƒŒƒCƒ„[‚Ì”íUŒ‚”»’è
	class CProcHitTest : public CProc {
	public:

		//!	@brief	”»’è‚Ìí—Ş
		enum EType {
			ATTACKER,	// UŒ‚‘¤
			DEFENDER,	// ”íUŒ‚‘¤

			TYPE_MAX
		};

		//!	@brief	“–‚½‚è”»’è‚ğ“o˜^‚·‚é
		void Register(CHolder& _rHolder, const EType _type);

	private:

		virtual void	SetupTypeCount() override { m_typeCount = TYPE_MAX; }
		virtual void	SetupCollisionMatrix() override;

		virtual	void	ProcCollide(SCollideData& _rColA, SCollideData& _rColB) override;
		virtual void	ProcBeforeCollide() override;
		virtual void	ProcAfterCollide() override;


		//!	@brief	ATTACKER‘¤‚ÆDEFENDER‘¤‚ğ“Á’è‚µ‚½‚¤‚¦‚Å‚Ìˆ—
		void ProcCollideAttack(SCollideData& _rColAttacker, SCollideData& _rColDefender);

	public:
		CProcHitTest() { }
		virtual ~CProcHitTest() { }
	};
}
