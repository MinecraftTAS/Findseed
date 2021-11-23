package de.pfannekuchen.tasdiscordbot;

import java.awt.Color;
import java.util.Properties;
import java.util.Random;

import javax.security.auth.login.LoginException;

import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.MessageBuilder;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.entities.MessageEmbed;
import net.dv8tion.jda.api.events.interaction.SlashCommandEvent;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import net.dv8tion.jda.api.interactions.commands.OptionMapping;
import net.dv8tion.jda.api.interactions.commands.OptionType;
import net.dv8tion.jda.api.interactions.commands.build.CommandData;
import net.dv8tion.jda.api.requests.GatewayIntent;
import net.dv8tion.jda.api.utils.MemberCachePolicy;

public class TASDiscordBot extends ListenerAdapter implements Runnable {

	private final JDA jda;
	private final Properties configuration;
	
	@Override
	public void onSlashCommand(SlashCommandEvent event) {
		if ("findseed".equalsIgnoreCase(event.getName()))
			event.reply(new MessageBuilder().setEmbeds(findseedEmbed(event.getOption("seed"), event.getUser().getAsTag(), event.getUser().getEffectiveAvatarUrl())).build()).complete();
	}
	
	private String[] broken = new String[] {
		"Great, you broke it.",
		"Wait? It's broken.. How did that happen?",
		"uhh that's weird",
		"Broken? Interesting, lets hope it doesn't leak end dust.",
		"OH NO IT'S BROKEN. CALL HEROBRINE",
		"asldzhiuaszd836q2w097rew",
		"1+2	4 mv9182n	3z037n5vnz 87923 t5",
		"20p3ic  nz745897b23zb5t 709823t5790n2305",
		"23789cn5 z609237c5 bt0ß92w349567m",
	};
	
	private MessageEmbed findseedEmbed(OptionMapping optionMapping, String usertag, String userurl) {
		boolean[] eyes = new boolean[12];
		Random rng = new Random();
		if (optionMapping != null) rng.setSeed(optionMapping.getAsLong());
		int eyeCount = 0;
		String nothing = "<:nothing:912781172063490050>";
		String full = "<:nothing:912780807091933184>";
		String empty = "<:nothing:912780807041613884>";
		String lava = "<:nothing:912779051926700063>";
		String broken2 = "<:end_portal_frame_broken2:912811133952466974>";
		String broken1 = "<:end_portal_frame_broken1:912811134191562812>";
		StringBuilder msg = new StringBuilder();
		msg.append(nothing);
		String footer = optionMapping != null ? "Used seed: " + optionMapping.getAsString() : "";
		for (int i = 0; i < eyes.length; i++) {
			eyes[i] = rng.nextFloat() < 0.1f;
			if (eyes[i]) eyeCount++;
			msg.append(eyes[i] ? full : empty);
			if (i == 2) msg.append(nothing);
			if (i == 2) msg.append("\n");
			if (i == 3) {
				msg.append(lava);
				msg.append(lava);
				msg.append(lava);
			}
			if (i == 4) msg.append("\n");
			if (i == 5) {
				msg.append(lava);
				msg.append(lava);
				msg.append(lava);
			}
			if (i == 6) msg.append("\n");
			if (i == 7) {
				msg.append(lava);
				msg.append(lava);
				msg.append(lava);
			}
			if (i == 8) {
				msg.append("\n");
				msg.append(nothing);
			}
			if (i == 11) msg.append(nothing);
			
		}
		String out = msg.toString();
		rng = new Random();
		boolean isBroken = rng.nextInt(10) == 5;
		if (eyeCount >= 12) {
			out = empty+full+full+full+nothing+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+nothing+full+full+full+empty;
			footer = "Too bad!";
			eyeCount = 0;
			rng.setSeed(optionMapping.getAsLong());
			for (int i = 0; i < 16; i++) {
				eyeCount += rng.nextFloat() < 0.1f ? 1 : 0;
			}
			if (eyeCount == 13) {
				out = empty+full+full+full+nothing+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+nothing+full+full+full+full;
			} else if (eyeCount == 14) {
				out = empty+full+full+full+empty+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+full+full+full+full+full;
				footer = "Dammit!";
			} else if (eyeCount == 15) {
				out = empty+full+full+full+full+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+full+lava+lava+lava+full+'\n'+full+full+full+full+full;
				footer = "Dammit!";
			}
		} else if (isBroken) {
			footer = broken[rng.nextInt(broken.length)];
			eyeCount = rng.nextInt(32)-64;
			out = out.replaceAll(full, rng.nextBoolean() ? broken1 : broken2);
		}
		return new EmbedBuilder().setTitle("findseed - Your seed is a **" + eyeCount + "** eye").addField("", out, false).setColor(Color.red.darker()).addField("", footer, false).setFooter(usertag, userurl).build();
	}

	public TASDiscordBot(Properties configuration) throws InterruptedException, LoginException {
		this.configuration = configuration;
		final JDABuilder builder = JDABuilder.createDefault(this.configuration.getProperty("token"))
				.setMemberCachePolicy(MemberCachePolicy.ALL)
                .enableIntents(GatewayIntent.GUILD_MEMBERS)
                .addEventListeners(this);
		this.jda = builder.build();
		this.jda.awaitReady();
	}

	@Override
	public void run() {
		/* Register the Commands */
		System.out.println("[Findseed] Preparing Bot...");
		CommandData d = new CommandData("findseed", "Finds a seed and shows the amount of eyes it has.");
		d.addOption(OptionType.INTEGER, "seed", "Seed to check on", false);
		for (Guild guild : jda.getGuilds()) {
			guild.upsertCommand(d).complete();
		}
		System.out.println("[Findseed] Done preparing bot.");
	}
}
