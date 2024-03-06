#include <stdlib.h>
#include <signal.h>

#include <concord/discord.h>
#include <concord/log.h>

/// Config file for the bot
#define CONFIG_FILE "config.json"

/// Empty emoji
#define NOTHING_EMOJI "<:nothing:912781172063490050>"
/// Full end portal frame emoji
#define FRAME_FULL_EMOJI "<:end_portal_frame_full:912780807091933184>"
/// Empty end portal frame emoji
#define FRAME_EMOJI "<:end_portal_frame:912780807041613884>"
/// Lava emoji
#define LAVA_EMOJI "<a:lava:912779051926700063>"
/// Void emoji (top left)
#define VOID_TL_EMOJI "<a:endportal1:968224841473855549>";
/// Void emoji (top middle)
#define VOID_TM_EMOJI "<a:endportal2:968224842224656424>";
/// Void emoji (top right)
#define VOID_TR_EMOJI "<a:endportal3:968224854639804457>";
/// Void emoji (middle left)
#define VOID_ML_EMOJI "<a:endportal4:968224854622998578>";
/// Void emoji (middle middle)
#define VOID_MM_EMOJI "<a:endportal5:968224854761414746>";
/// Void emoji (middle right)
#define VOID_MR_EMOJI "<a:endportal6:968224839246696448>";
/// Void emoji (bottom left)
#define VOID_BL_EMOJI "<a:endportal7:968224855826763926>";
/// Void emoji (bottom middle)
#define VOID_BM_EMOJI "<a:endportal8:968224856283951116>";
/// Void emoji (bottom right)
#define VOID_BR_EMOJI "<a:endportal9:968224854161645590>";

/// Mask for 48 bits
#define MASK_48_BITS 0xFFFFFFFFFFFFULL
/// Multiplier for the seed
#define MULTIPLIER 0x5DEECE66DULL
/// Addend for the seed
#define ADDEND 0xBULL

/// Discord client instance
static struct discord *discord_client = NULL;

/**
 * Generate the next float from the seed (Java Random)
 *
 * \param seed Seed
 *   The seed to generate the next float from
 *
 * \return The next float
 *   The next float generated from the seed
*/
static float nextFloat(uint64_t *seed) {
    *seed = (*seed * MULTIPLIER + ADDEND) & MASK_48_BITS;
    int next = (int) ((int64_t) *seed >> (48ULL - 24ULL));
    float result = next / (float) (1 << 24);
    return result;
}

/**
 * Handle SIGINT signal and shut down the bot
 *
 * \param signum Signal number
 */
static void handle_sigint(int signum) {
    log_info("[FINDSEED] Received SIGINT, shutting down bot...");
    if (discord_client) discord_shutdown(discord_client);
}

/**
 * Initialize discord client
 *
 * \return 0 on success, 1 on failure
 */
static int initialize_discord() {
    // initialize concord
    CCORDcode code = ccord_global_init();
    if (code) {
        log_trace("[FINDSEED] ccord_global_init() failed: %d", code);

        return 1;
    }
    log_trace("[FINDSEED] ccord_global_init() success");

    // create discord client
    discord_client = discord_config_init(CONFIG_FILE);
    if (!discord_client) {
        log_trace("[FINDSEED] discord_create() failed");

        ccord_global_cleanup();
        return 1;
    }
    log_trace("[FINDSEED] discord_create() success");

    return 0;
}

/**
 * Handle findseed interaction
 *
 * \param client Discord client
 * \param event Interaction event
 */
static void on_findseed(struct discord *client, const struct discord_interaction *event) {
    // get seed
    srand(time(NULL));
    int64_t seed = ((int64_t) rand() << 32) | (int64_t) rand();
    if (event->data->options) seed = (int64_t) atoll(event->data->options->array[0].value);
    uint64_t java_seed = (uint64_t) seed & MASK_48_BITS; // mask 48 bits

    // calculate amount of eyes
    int eye_count = 0;
    int special_eye_count = 0;
    char* frame_emojis[15];
    for (int i = 0; i < 15; i++) {
        if (nextFloat(&java_seed) < 0.1) {
            frame_emojis[i] = FRAME_FULL_EMOJI;
            special_eye_count++;
            if (i < 12) eye_count++;
        } else
            frame_emojis[i] = FRAME_EMOJI;
    }

    // create lava/void emojis
    char* lava_emojis[9];
    if (eye_count == 12) {
        lava_emojis[0] = VOID_TL_EMOJI;
        lava_emojis[1] = VOID_TM_EMOJI;
        lava_emojis[2] = VOID_TR_EMOJI;
        lava_emojis[3] = VOID_ML_EMOJI;
        lava_emojis[4] = VOID_MM_EMOJI;
        lava_emojis[5] = VOID_MR_EMOJI;
        lava_emojis[6] = VOID_BL_EMOJI;
        lava_emojis[7] = VOID_BM_EMOJI;
        lava_emojis[8] = VOID_BR_EMOJI;
    } else {
        // i'm crazy, i know
        lava_emojis[0] = lava_emojis[1] = lava_emojis[2] = lava_emojis[3] = lava_emojis[4] = lava_emojis[5] = lava_emojis[6] = lava_emojis[7] = lava_emojis[8] = LAVA_EMOJI;
    }

    // create corner emojis
    char* corner_emojis[4];
    corner_emojis[0] = corner_emojis[1] = corner_emojis[2] = corner_emojis[3] = NOTHING_EMOJI;
    if (eye_count == 12) {
        if (special_eye_count >= 13) {
            corner_emojis[0] = frame_emojis[12];
            lava_emojis[2] = LAVA_EMOJI;
        }
        if (special_eye_count >= 14) {
            corner_emojis[3] = frame_emojis[13];
            lava_emojis[5] = LAVA_EMOJI;
            lava_emojis[6] = LAVA_EMOJI;
        }
        if (special_eye_count >= 15) {
            lava_emojis[1] = LAVA_EMOJI;
            lava_emojis[2] = FRAME_EMOJI;
            lava_emojis[3] = FRAME_EMOJI;
            lava_emojis[8] = FRAME_FULL_EMOJI;
            frame_emojis[6] = FRAME_FULL_EMOJI FRAME_EMOJI;
        }
    }

    // create end portal
    char end_portal_message[2001];
    sprintf(end_portal_message, "%s%s%s%s%s\n%s%s%s%s%s\n%s%s%s%s%s\n%s%s%s%s%s\n%s%s%s%s%s",
        corner_emojis[0], frame_emojis[0], frame_emojis[1], frame_emojis[2], corner_emojis[1],
        frame_emojis[3], lava_emojis[0], lava_emojis[1], lava_emojis[2], frame_emojis[4],
        frame_emojis[5], lava_emojis[3], lava_emojis[4], lava_emojis[5], frame_emojis[6],
        frame_emojis[7], lava_emojis[6], lava_emojis[7], lava_emojis[8], frame_emojis[8],
        corner_emojis[2], frame_emojis[9], frame_emojis[10], frame_emojis[11], corner_emojis[3]
    );

    // create fields
    char value[512];
    sprintf(value, "%s %ld", event->data->options ? "Seed specified:" : "Random seed:", seed);
    struct discord_embed_field fields[] = {
        {
            .name = "",
            .value = end_portal_message,
            .Inline = false
        },
        {
            .name = "",
            .value = value,
            .Inline = false
        }
    };

    // create embed
    char avatar[256];
    sprintf(avatar, "https://cdn.discordapp.com/avatars/%"PRIu64"/%s.webp", event->member->user->id, event->member->user->avatar);
    char title[256];
    sprintf(title, "findseed - Your seed is a %d eye", eye_count);
    struct discord_embed embeds[] = {
        {
            .title = title,
            .color = 0x3f6965,
            .footer = &(struct discord_embed_footer) {
                .text = event->member->user->username,
                .icon_url = avatar
            },
            .fields = &(struct discord_embed_fields) {
                .size = sizeof(fields) / sizeof *fields,
                .array = fields
            }
        }
    };

    discord_create_interaction_response(client, event->id, event->token, &(struct discord_interaction_response) {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &(struct discord_interaction_callback_data) {
            .content = "",
            .embeds = &(struct discord_embeds) {
                .size = sizeof(embeds) / sizeof *embeds,
                .array = embeds
            }
        }
    }, NULL);
}

/**
 * Main bot function
 *
 * \param client Discord client
 * \param event Ready event
 */
void bot_main(struct discord *client, const struct discord_ready *event) {
    // initialize global slash commands
    log_info("[FINDSEED] Initializing global slash commands...");
    discord_set_on_interaction_create(client, on_findseed);
    discord_bulk_overwrite_global_application_commands(client, event->application->id, &(struct discord_application_commands) {
        .size = 1,
        .array = &(struct discord_application_command) {
            .type = DISCORD_APPLICATION_CHAT_INPUT,
            .name = "findseed",
            .description = "Find the amount of eyes in the end portal",
            .default_permission = true,
            .application_id = event->application->id,
            .options = &(struct discord_application_command_options) {
                .size = 1,
                .array = &(struct discord_application_command_option) {
                    .type = DISCORD_APPLICATION_OPTION_INTEGER,
                    .name = "seed",
                    .description = "The seed to find the amount of eyes in",
                    .required = false
                }
            }
        }
    }, NULL);
}

/**
 * Main function
 *
 * \return 0 on success, 1 on failure
 */
int main() {
    // initialize discord bot
    log_info("[FINDSEED] Initializing findseed discord bot...");
    if (initialize_discord()) {
        log_fatal("[FINDSEED] Failed to initialize discord bot");

        return EXIT_FAILURE;
    }

    // run discord bot
    log_info("[FINDSEED] Launching findseed discord bot...");
    signal(SIGINT, handle_sigint);
    discord_set_on_ready(discord_client, bot_main);
    CCORDcode code = discord_run(discord_client);

    // cleanup discord bot
    log_info("[FINDSEED] Discord bot exited (%d), cleaning up...", code);
    discord_cleanup(discord_client);
    ccord_global_cleanup();
    return EXIT_SUCCESS;
}
