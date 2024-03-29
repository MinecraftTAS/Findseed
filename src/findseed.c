/**
 * Copyright (C) 2024  Pancake
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <signal.h>
#include <string.h>

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
#define VOID_TL_EMOJI "<a:endportal1:968224841473855549>"
/// Void emoji (top middle)
#define VOID_TM_EMOJI "<a:endportal2:968224842224656424>"
/// Void emoji (top right)
#define VOID_TR_EMOJI "<a:endportal3:968224854639804457>"
/// Void emoji (middle left)
#define VOID_ML_EMOJI "<a:endportal4:968224854622998578>"
/// Void emoji (middle middle)
#define VOID_MM_EMOJI "<a:endportal5:968224854761414746>"
/// Void emoji (middle right)
#define VOID_MR_EMOJI "<a:endportal6:968224839246696448>"
/// Void emoji (bottom left)
#define VOID_BL_EMOJI "<a:endportal7:968224855826763926>"
/// Void emoji (bottom middle)
#define VOID_BM_EMOJI "<a:endportal8:968224856283951116>"
/// Void emoji (bottom right)
#define VOID_BR_EMOJI "<a:endportal9:968224854161645590>"
/// Full broken end portal frame emoji
#define FRAME_FULL_BROKEN_EMOJI "<:end_portal_frame_broken1:912811134191562812>"
/// Broken end portal frame emoji
#define FRAME_BROKEN_EMOJI "<:end_portal_frame_broken2:912811133952466974>"

/// Corrupted texts
#define CORRUPTED_TEXTS { \
    "Great, you broke it.", \
    "Wait? It's broken?!", \
    "Okay.. let's pretend we never saw this", \
    "Huh, that's weird. Let's hope it doesn't cause any trouble" \
    "I don't really know what happened either", \
    "I'm not sure what you did, but it's not supposed to look like this", \
    "This is not how it's supposed to look like", \
    "Let's just pretend this never happened", \
}
#define CORRUPTED_TEXTS_SIZE 8

/// Mask for 48 bits
#define MASK_48_BITS 0xFFFFFFFFFFFFULL
/// Multiplier for the seed
#define MULTIPLIER 0x5DEECE66DULL
/// Addend for the seed
#define ADDEND 0xBULL

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
 * Generate zalgo text
 *
 * \param text Text to corrupt
 * \param corrupted Corrupted text
 *
 * \return The original text
 */
static char* corrupt_text(char* text, char* corrupted) {
    int len = strlen(text);
    char* corrupted_ptr = corrupted;

    for (int i = 0; i < len; i++) {
        *corrupted_ptr++ = text[i];

        uint32_t n  = (uint32_t) rand() % 2;
        while (n--) {
            uint32_t base = 0x0300;
            uint32_t acc = (uint32_t) rand() % 112;
            uint32_t code = base + acc;
            *corrupted_ptr++ = 0xc0 | ((code >> 6) & 0x1f);
            *corrupted_ptr++ = 0x80 | (code & 0x3f);
        }

    }

    *corrupted_ptr = 0;

    return text;
}

/**
 * Handle SIGINT signal and shut down the bot
 *
 * \param signum Signal number
 */
static void handle_sigint(int signum) {
    log_info("[FINDSEED] Received SIGINT, shutting down bot...");
    ccord_shutdown_async();
}

/**
 * Initialize discord client
 *
 * \return Discord client on success, NULL on failure
 */
static struct discord* initialize_discord() {
    // initialize concord
    CCORDcode code = ccord_global_init();
    if (code) {
        log_trace("[FINDSEED] ccord_global_init() failed: %d", code);

        return NULL;
    }
    log_trace("[FINDSEED] ccord_global_init() success");

    // create discord client
    struct discord* client = discord_config_init(CONFIG_FILE);
    if (!client) {
        log_trace("[FINDSEED] discord_create() failed");

        ccord_global_cleanup();
        return NULL;
    }
    log_trace("[FINDSEED] discord_create() success");

    return client;
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
        char* void_emojis[] = {
            VOID_TL_EMOJI, VOID_TM_EMOJI, VOID_TR_EMOJI,
            VOID_ML_EMOJI, VOID_MM_EMOJI, VOID_MR_EMOJI,
            VOID_BL_EMOJI, VOID_BM_EMOJI, VOID_BR_EMOJI
        };
        memcpy(lava_emojis, void_emojis, sizeof(void_emojis));
    } else {
        for (int i = 0; i < 9; i++) lava_emojis[i] = LAVA_EMOJI;
    }

    // create corner emojis
    char message[128] = "";
    char* corner_emojis[4];
    corner_emojis[0] = corner_emojis[1] = corner_emojis[2] = corner_emojis[3] = NOTHING_EMOJI;
    if (eye_count == 12) {
        if (special_eye_count >= 12) {
            sprintf(message, "You actually did it. Unbelievable\n");
        }

        if (special_eye_count >= 13) {
            corner_emojis[0] = frame_emojis[12];
            lava_emojis[2] = LAVA_EMOJI;
            sprintf(message, "Wait, how did you...?\n");
        }

        if (special_eye_count >= 14) {
            corner_emojis[3] = frame_emojis[13];
            lava_emojis[5] = LAVA_EMOJI;
            lava_emojis[6] = LAVA_EMOJI;
            sprintf(message, "You're not supposed to see this\n");
        }

        if (special_eye_count >= 15) {
            lava_emojis[1] = LAVA_EMOJI;
            lava_emojis[2] = FRAME_EMOJI;
            lava_emojis[3] = FRAME_EMOJI;
            lava_emojis[8] = FRAME_FULL_EMOJI;
            frame_emojis[6] = FRAME_FULL_EMOJI FRAME_EMOJI;
            sprintf(message, "You broke it... You actually broke it\n");
        }
    }

    // check weird eye
    bool weird_eye = false;
    if (rand() <= RAND_MAX / 10000 && !event->data->options) {
        weird_eye = true;
        for (int i = 0; i < 12; i++)
            frame_emojis[i] = FRAME_FULL_EMOJI;
        char* void_emojis[] = {
            VOID_TL_EMOJI, VOID_TM_EMOJI, VOID_TR_EMOJI,
            VOID_ML_EMOJI, VOID_MM_EMOJI, VOID_MR_EMOJI,
            VOID_BL_EMOJI, VOID_BM_EMOJI, VOID_BR_EMOJI
        };
        memcpy(lava_emojis, void_emojis, sizeof(void_emojis));
        corner_emojis[0] = FRAME_FULL_EMOJI;
        corner_emojis[1] = FRAME_FULL_EMOJI;
        corner_emojis[2] = FRAME_FULL_EMOJI;
        corner_emojis[3] = FRAME_FULL_EMOJI;
        seed = ((int64_t) rand() << 32) | (int64_t) rand();
        eye_count = 12;
        special_eye_count = 16;
        sprintf(message, "They said it was impossible. How'd you do it?\n");
    }

    // check corrupted portal
    if (rand() <= RAND_MAX / 200 && !event->data->options) {
        char* corrupted_texts[] = CORRUPTED_TEXTS;
        int index = (uint32_t) rand() % CORRUPTED_TEXTS_SIZE;
        sprintf(message, "%s\n", corrupted_texts[index]);

        for (int i = 0; i < 15; i++) {
            if (rand() <= RAND_MAX / 2)
                frame_emojis[i] = (strcmp(frame_emojis[i], FRAME_FULL_EMOJI) == 0) ? FRAME_FULL_BROKEN_EMOJI : FRAME_BROKEN_EMOJI;
        }

        weird_eye = true;
    }

    // create end portal
    char end_portal_message[2001];
    sprintf(end_portal_message, "%s%s%s%s%s\n%s%s%s%s%s\n%s%s%s%s%s\n%s%s%s%s%s\n%s%s%s%s%s",
        corner_emojis[0],  frame_emojis[0],  frame_emojis[1],  frame_emojis[2],  corner_emojis[1],
        frame_emojis[3],   lava_emojis[0],   lava_emojis[1],   lava_emojis[2],   frame_emojis[4],
        frame_emojis[5],   lava_emojis[3],   lava_emojis[4],   lava_emojis[5],   frame_emojis[6],
        frame_emojis[7],   lava_emojis[6],   lava_emojis[7],   lava_emojis[8],   frame_emojis[8],
        corner_emojis[2],  frame_emojis[9],  frame_emojis[10], frame_emojis[11], corner_emojis[3]
    );

    // create fields
    char footer[2001];
    sprintf(footer, "%s%s %ld", message, event->data->options ? "Seed specified:" : "Random seed:", seed);
    if (weird_eye) free(corrupt_text(strdup(footer), footer));
    struct discord_embed_field fields[] = {
        {
            .name = "",
            .value = end_portal_message,
            .Inline = false
        },
        {
            .name = "",
            .value = footer,
            .Inline = false
        }
    };

    // create embed
    char avatar[256];
    sprintf(avatar, "https://cdn.discordapp.com/avatars/%"PRIu64"/%s.webp", event->member->user->id, event->member->user->avatar);
    char title[256];
    sprintf(title, "findseed - Your seed is a %d eye", eye_count >= 12 ? special_eye_count : eye_count);
    if (weird_eye) free(corrupt_text(strdup(title), title));
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

    log_info("[FINDSEED] User %s requested seed %ld (%s), responded with %d eye(s). Portal was %smarked as corrupted. Portal was %smarked as 16 eye.", event->member->user->username, seed, event->data->options ? "specified" : "random", eye_count == 12 ? special_eye_count : eye_count, weird_eye ? "" : "not ", special_eye_count == 16 ? "" : "not ");
}

bool is_initialized = false; ///< Whether the bot was initialized

/**
 * Main bot function
 *
 * \param client Discord client
 * \param event Ready event
 */
void bot_main(struct discord *client, const struct discord_ready *event) {
    // check if bot was already initialized
    if (is_initialized) return;
    is_initialized = true;

    // initialize global slash commands
    log_info("[FINDSEED] Initializing global slash commands...");
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
    struct discord* client = initialize_discord();
    if (!client) {
        log_fatal("[FINDSEED] Failed to initialize discord bot");

        return EXIT_FAILURE;
    }

    // run discord bot
    log_info("[FINDSEED] Launching findseed discord bot...");
    signal(SIGINT, handle_sigint);
    discord_set_on_ready(client, bot_main);
    discord_set_on_interaction_create(client, on_findseed);
    CCORDcode code = discord_run(client);

    // cleanup discord bot
    log_info("[FINDSEED] Discord bot exited (%d), cleaning up...", code);
    discord_cleanup(client);
    ccord_global_cleanup();
    return EXIT_SUCCESS;
}
