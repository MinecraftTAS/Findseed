#include <stdlib.h>
#include <signal.h>

#include <concord/discord.h>
#include <concord/log.h>

/// Config file for the bot
#define CONFIG_FILE "config.json"

/// Discord client instance
static struct discord *discord_client = NULL;

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

    (void) seed;
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
