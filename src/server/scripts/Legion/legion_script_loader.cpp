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

// This is where scripts' loading functions should be declared:
	
	//Zone Demon Hunter
void AddSC_zone_demon_hunter();

	//World Boss
void AddSC_world_boss();

// Maw of Souls
void AddSC_instance_maw_of_souls();
void AddSC_boss_ymiron_maw();
void AddSC_boss_harbaron_maw();
void AddSC_boss_helya_maw();


// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddLegionScripts()
{
	//Zone Demon Hunter
	AddSC_zone_demon_hunter();
	
	//World Boss
	AddSC_world_boss();
	
	// Maw of Souls
	AddSC_instance_maw_of_souls();
	AddSC_boss_ymiron_maw();
	AddSC_boss_harbaron_maw();
	AddSC_boss_helya_maw();
}
