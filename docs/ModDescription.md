## Description

This is a SKSE plugin completely overhauling thievery gameplay. It features lockpicking and pickpocketing timer, a thief reputation system and an Infamy System. 
It's designed to make thievery a bit more challenging but also more rewarding while also being as compatible with other mods as possible.

### Lockpicking

- Timed Lockpicking: You are getting kicked out of the lockpicking menu after a set time, based on your lockpicking skill level.
- Removes lockpick from your inventory as if you broke it. It also triggers basically the same reaction the game does for a lockpick related crime.
- Dynamic Lockpicking: The higher your infamy level gets, the easier locks are to pick, but you're also easier to spot, keep that in mind.

### Pickpocketing

- Timed Pickpocketing: You are getting kicked out of the pickpocket inventory after a set time, based on your pickpocket skill and whether or not the target is sleeping.
- Dynamic Pickpocket cap: changes the pickpocket cap based on your Infamy level. The higher it is, the harder pickpocketing gets down to 50% maximum chance. It also adjusts the overall pickpocket chance ± 15% based on the infamy level.
- Default Pickpocket max chance is set to 75% (instead of 90%) but if you have all 5 reputation perks, it can go up to 100%

### Reputation

- Thief reputation: When stealing valuable items at night you build up reputation that opens up great ways to get rich. Note that if you're caught for any crime, that reputation will decrease. 
- Infamy System: Similar but opposite to the reputation system is the Infamy System. Anytime you get a bounty, your infamy level rises and makes sneaking near guards and pickpocketing much harder. Guards may randomly attack you because they know you. Infamy decreases over time.
- Reputation Perks:
    - 80 reputation: Gold Rush: Whenever you enter a cell, valuable items and containers will be highlighted for you.
    - 150 reputation: Sleight of Hand: You can pickpocket items from NPCs by bumping into them. Note, your chance of success is lowered this way.
    - 210 reputation: Friends in High Places: Your non-violent bounties are reduced over time.
    - 260 reputation: Shady Merchant: Sell stolen items for 20% more gold.
    - 310 reputation: 10% chance to not get a bounty for minor, non-violent crimes.