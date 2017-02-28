/*
* Copyright (C) 2014-2017 StormCore
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "maw_of_souls.h"

enum Auras {
	AURA_FRAGMENT_PLAYER = 194327,
};

enum Visuals {
	FRAGMENT_VISUAL = 198551,
};

enum Spells {

	// Harbaron Spells
	SPELL_FRAGMENT = 194325,
	SPELL_SERVITOR = 194231,
	SPELL_SCYTHE = 194216,
	SPELL_RIP = 194235,

	// Shackled Servitor Spells
	SPELL_VOID_SNAP = 194266,
};

enum Yells {
	YELL_PRE_ENCOUNTER = 0,
	YELL_ENTER_COMBAT = 1,
	YELL_SCYTHE = 2,
	YELL_FRAGMENT = 3,
	YELL_SERVITOR = 4,
	YELL_KILL = 5,
	YELL_KILLED = 6
};

enum Events {
	EVENT_FRAGMENT = 1,
	EVENT_SERVITOR = 2,
	EVENT_SCYTHE = 3,
	EVENT_RIP = 4
};

enum Phases {
	PHASE_INTRO = 0,
	PHASE_READY = 1
};


class boss_harbaron_maw : public CreatureScript {
	public:
		boss_harbaron_maw() : CreatureScript("boss_harbaron_maw") {}

		struct boss_harbaron_mawAI : public BossAI {
			boss_harbaron_mawAI(Creature* creature) : BossAI(creature, DATA_HARBARON_MAW) {
				instance = me->GetInstanceScript();
				getDifficuly();
				Initialize();
			}

			void MoveInLineOfSight(Unit* who) override {
				if(who->GetTypeId() == TYPEID_PLAYER && me->GetDistance2d(who) < 35.0f && !me->IsInCombat() && !alreadyWelcomed) {
					alreadyWelcomed = true;
					Talk(YELL_PRE_ENCOUNTER);
				}
			}

			void Initialize() {
				events.ScheduleEvent(EVENT_SCYTHE, 5 * IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_SERVITOR, 7 * IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_FRAGMENT, 19 * IN_MILLISECONDS);
			}

			void Reset() override {
				BossAI::Reset();
				events.Reset();
				Initialize();
				instance->SetBossState(DATA_HARBARON_MAW, FAIL);
			}

			void EnterCombat(Unit* who) {
				BossAI::EnterCombat(who);
				Talk(YELL_ENTER_COMBAT);
				instance->SetBossState(DATA_HARBARON_MAW, IN_PROGRESS);
			}

			void JustDied(Unit* killer) override {
				BossAI::JustDied(killer);
				Talk(YELL_KILLED);
				instance->SetBossState(DATA_HARBARON_MAW, DONE);
			}

			void KilledUnit(Unit* victim) override {
				if(victim->GetTypeId() == TYPEID_PLAYER)
					Talk(YELL_KILL);
			}

			void UpdateAI(uint32 diff) override {

				if(!UpdateVictim())
					return;

				events.Update(diff);

				if(me->HasUnitState(UNIT_STATE_CASTING))
					return;

				while(uint32 eventID = events.ExecuteEvent()) {
					switch(eventID) {
						case EVENT_SERVITOR:
							Talk(YELL_SERVITOR);
							if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true)) {
								DoCast(target, SPELL_SERVITOR);
							}
							events.ScheduleEvent(EVENT_SERVITOR, 23 * IN_MILLISECONDS);
						break;

						case EVENT_FRAGMENT:
							Talk(YELL_FRAGMENT);
							if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true)) {
								DoCast(target, FRAGMENT_VISUAL);
								me->AddAura(AURA_FRAGMENT_PLAYER, target);
							}
							events.ScheduleEvent(EVENT_FRAGMENT, 30 * IN_MILLISECONDS);
						break;

						case EVENT_SCYTHE:
							DoCastVictim(SPELL_SCYTHE);
							events.ScheduleEvent(EVENT_SCYTHE, 10 * IN_MILLISECONDS);
						break;

						default:
							break;
					}
				}

				DoMeleeAttackIfReady();
			}

			void getDifficuly() {
				Difficulty instanceDifficulty = instance->instance->GetDifficultyID();
				uint64 myHealth = me->GetHealth();
				double healthModifier;
				double damageModifier;

				if(instanceDifficulty == DIFFICULTY_NORMAL) {
					healthModifier = 1.0;
					damageModifier = 1.0;
				} else if(instanceDifficulty == DIFFICULTY_HEROIC) {
					healthModifier = 1.0571428043; // HealthModifier 36.99
					damageModifier = 1.39285714286;
					isHeroicOrMythic = true;
				} else if(instanceDifficulty == DIFFICULTY_MYTHIC) {
					healthModifier = 1.32142855675; // HealthModifier 46.25
					damageModifier = 1.95;
					isHeroicOrMythic = true;
				} else {
					healthModifier = 20;
					damageModifier = 20;
				}
				uint64 difficultyAdjustedHealth = static_cast<unsigned long long>(myHealth * healthModifier);
				me->SetMaxHealth(difficultyAdjustedHealth);
				me->SetHealth(difficultyAdjustedHealth);
				me->SetModifierValue(UNIT_MOD_DAMAGE_MAINHAND, BASE_VALUE, static_cast<float>(damageModifier));
			}

			float getRandomFloat(float a, float b) {
				float random = ((float) rand()) / (float) RAND_MAX;
				float diff = b - a;
				float r = random * diff;
				return a + r;
			}

			private:
				bool alreadyWelcomed = false;
				InstanceScript* instance;
				bool isHeroicOrMythic = false;
		};

		CreatureAI* GetAI(Creature* creature) const override {
			return GetMawOfSoulsAI<boss_harbaron_mawAI>(creature);
		}
};

class npc_shackled_servitor : public CreatureScript {
public:
	npc_shackled_servitor() : CreatureScript("npc_shackled_servitor") {}
	struct npc_shackled_servitorAI : public ScriptedAI {
		npc_shackled_servitorAI(Creature* creature) : ScriptedAI(creature) {
			instance = me->GetInstanceScript();
			Initialize();
			SetCombatMovement(false);
		}

		void Initialize() {
			voidSnapCastEach = _voidSnapCastEach;
		}

		void UpdateAI(uint32 diff) override {
			if(!UpdateVictim())
				return;

			if(voidSnapCastEach <= diff) {
				DoCastAOE(SPELL_VOID_SNAP);
			} else voidSnapCastEach -= diff;
		}

	private:
		InstanceScript* instance;
		uint32 voidSnapCastEach;
		uint32 _voidSnapCastEach = 6 * IN_MILLISECONDS;
	};

	CreatureAI* GetAI(Creature* creature) const override {
		return GetMawOfSoulsAI<npc_shackled_servitorAI>(creature);
	}
};


void AddSC_boss_harbaron_maw() {
	new boss_harbaron_maw();
	new npc_shackled_servitor();
}
