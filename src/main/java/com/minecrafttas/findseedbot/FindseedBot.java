package com.minecrafttas.findseedbot;

import java.util.Properties;
import java.util.Random;

import javax.security.auth.login.LoginException;

import net.dv8tion.jda.api.EmbedBuilder;
import net.dv8tion.jda.api.JDA;
import net.dv8tion.jda.api.JDABuilder;
import net.dv8tion.jda.api.entities.Guild;
import net.dv8tion.jda.api.entities.MessageEmbed;
import net.dv8tion.jda.api.events.interaction.command.SlashCommandInteractionEvent;
import net.dv8tion.jda.api.hooks.ListenerAdapter;
import net.dv8tion.jda.api.interactions.commands.OptionType;
import net.dv8tion.jda.api.interactions.commands.build.OptionData;
import net.dv8tion.jda.api.requests.GatewayIntent;
import net.dv8tion.jda.api.requests.restaction.CommandListUpdateAction;
import net.dv8tion.jda.api.utils.MemberCachePolicy;
import net.dv8tion.jda.api.utils.messages.MessageCreateBuilder;
import net.dv8tion.jda.internal.interactions.CommandDataImpl;

public class FindseedBot extends ListenerAdapter implements Runnable {

	private final JDA jda;
	private final Properties configuration;
	
	@Override
	public void onSlashCommandInteraction(SlashCommandInteractionEvent event) {
		if ("findseed".equalsIgnoreCase(event.getName())) {
			
			long seedOption=0;
			boolean isSetSeed=false;
			if(event.getOption("seed")!=null) {
				try {
					seedOption = event.getOption("seed").getAsLong();
				} catch (NumberFormatException e) {
					event.reply("Invalid seed!").setEphemeral(true).queue();
					return;
				}
				isSetSeed=true;
			}
			
			String userTagOption= event.getUser().getAsTag();
			String avatarUrl=event.getUser().getEffectiveAvatarUrl();
			
			
			event.reply(new MessageCreateBuilder().setEmbeds(findseedEmbed(seedOption, userTagOption, avatarUrl, isSetSeed)).build()).complete();
		}
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
	
	private MessageEmbed findseedEmbed(long seed, String usertag, String userurl, boolean isSetSeed) {
		boolean[] eyes = new boolean[12];
		Random rng = new Random();
		if (isSetSeed) rng.setSeed(seed ^ 0x5deece66dL);
		int eyeCount = 0;
		String n = "<:nothing:912781172063490050>";
		String o = "<:nothing:912780807091933184>";
		String e = "<:nothing:912780807041613884>";
		String l = "<a:lava:912779051926700063>";
		String e1= "<a:endportal1:968224841473855549>";
		String e2= "<a:endportal2:968224842224656424>";
		String e3= "<a:endportal3:968224854639804457>";
		String e4= "<a:endportal4:968224854622998578>";
		String e5= "<a:endportal5:968224854761414746>";
		String e6= "<a:endportal6:968224839246696448>";
		String e7= "<a:endportal7:968224855826763926>";
		String e8= "<a:endportal8:968224856283951116>";
		String e9= "<a:endportal9:968224854161645590>";
		String broken2 = "<:end_portal_frame_broken2:912811133952466974>";
		String broken1 = "<:end_portal_frame_broken1:912811134191562812>";
		StringBuilder msg = new StringBuilder();
		msg.append(n);
		String footer = isSetSeed ? "Used seed: " + seed : "Random seed: " + getSeed(rng);
		for (int i = 0; i < eyes.length; i++) {
			eyes[i] = rng.nextFloat() < 0.1f;
			if (eyes[i]) eyeCount++;
			msg.append(eyes[i] ? o : e);
			if (i == 2) msg.append(n);
			if (i == 2) msg.append("\n");
			if (i == 3) {
				msg.append(l);
				msg.append(l);
				msg.append(l);
			}
			if (i == 4) msg.append("\n");
			if (i == 5) {
				msg.append(l);
				msg.append(l);
				msg.append(l);
			}
			if (i == 6) msg.append("\n");
			if (i == 7) {
				msg.append(l);
				msg.append(l);
				msg.append(l);
			}
			if (i == 8) {
				msg.append("\n");
				msg.append(n);
			}
			if (i == 11) msg.append(n);
			
		}
		String out = msg.toString();
		rng = new Random();
		boolean isBroken = rng.nextInt(10) == 5;
		if (eyeCount >= 12) {
			out = n+o+o+o+n+'\n'
				+o+e1+e2+e3+o+'\n'
				+o+e4+e5+e6+o+'\n'
				+o+e7+e8+e9+o+'\n'
				+n+o+o+o+n;
			eyeCount = 0;
			rng.setSeed(seed ^ 0x5deece66dL);
			for (int i = 0; i < 16; i++) {
				eyeCount += (rng.nextFloat() < 0.1f ? 1 : 0);
			}
			if (eyeCount == 13) {
				out = o+o+o+o+n+'\n'
					+o+e1+e2+l+o+'\n'
					+o+e4+e5+e6+o+'\n'
					+o+e7+e8+e9+o+'\n'
					+n+o+o+o+n;
				footer="Wait, how did you?\n"+ footer;
			} else if (eyeCount == 14) {
				out = o+o+o+o+n+'\n'
					+o+e1+e2+l+o+'\n'
					+o+e4+e5+l+o+'\n'
					+o+l+e8+e9+o+'\n'
					+n+o+o+o+o;
				footer = "Again? 14 eyes? Really?\n"+footer;
			} else if (eyeCount == 15) {
				out = o+o+o+o+n+'\n'
					+o+e1+l+e+o+'\n'
					+o+e+e5+l+o+e+'\n'
					+o+l+e8+o+o+'\n'
					+n+o+o+o+o;
				footer = "Well, now it's really messed up...\n"+footer;
			}
		} else if (isBroken) {
			footer = broken[rng.nextInt(broken.length)];
			eyeCount = rng.nextInt(32)-64;
			out = out.replaceAll(o, rng.nextBoolean() ? broken1 : broken2);
		}
		return new EmbedBuilder().setTitle("findseed - Your seed is a **" + eyeCount + "** eye").addField("", out, false).setColor(0x3f6965).addField("", footer, false).setFooter(usertag, userurl).build();
	}

	public FindseedBot(Properties configuration) throws InterruptedException, LoginException {
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
		for (Guild guild : jda.getGuilds()) {
			
			CommandListUpdateAction updater = guild.updateCommands();
			
			CommandDataImpl findseedcommand = new CommandDataImpl("findseed", "Finds a seed and shows the amount of eyes it has.");
			OptionData seedOption=new OptionData(OptionType.STRING, "seed", "Seed to check on", false);
			
			findseedcommand.addOptions(seedOption);
			
			updater.addCommands(findseedcommand);
			updater.queue();
		}
		System.out.println("[Findseed] Done preparing bot.");
	}
	
    public long getSeed(Random rng) {
        long seed = reverse(rng.nextLong()) ^ 0x5deece66dL;
        rng.setSeed(seed);
        return seed^ 0x5deece66dL;
    }

    public static long reverse(long in) {
        return (((7847617*((24667315*(in >>> 32) + 18218081*(in & 0xffffffffL) + 67552711) >> 32) - 18218081*((-4824621*(in >>> 32) + 7847617*(in & 0xffffffffL) + 7847617) >> 32)) - 11) * 246154705703781L) & 0xffffffffffffL;
    }
}
