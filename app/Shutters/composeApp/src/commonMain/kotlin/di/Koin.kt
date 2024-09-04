package di

import automations.AutomationsScreenModel
import controls.ControlsScreenModel
import data.LocalSettings
import data.ShutterServiceImpl
import data.ShuttersApi
import io.ktor.client.HttpClient
import io.ktor.client.plugins.HttpSend
import io.ktor.client.plugins.HttpTimeout
import io.ktor.client.plugins.contentnegotiation.ContentNegotiation
import io.ktor.client.plugins.defaultRequest
import io.ktor.client.plugins.logging.LogLevel
import io.ktor.client.plugins.logging.Logger
import io.ktor.client.plugins.logging.Logging
import io.ktor.client.plugins.plugin
import io.ktor.http.ContentType
import io.ktor.http.URLBuilder
import io.ktor.http.headers
import io.ktor.http.takeFrom
import io.ktor.serialization.kotlinx.json.json
import kotlinx.serialization.json.Json
import org.koin.core.context.startKoin
import org.koin.dsl.module
import settings.SettingsScreenModel

fun initKoin() = startKoin {
//    androidContext(this@MainApplication)
    modules(appModule + networkModule)
}

private val appModule = module {
    single<ShuttersApi> { ShutterServiceImpl(get(), get()) }
    factory { AutomationsScreenModel(get()) }
    factory { ControlsScreenModel(get()) }
    factory { SettingsScreenModel(get()) }
}

private val networkModule = module {
    single { LocalSettings() }
    single { httpClientFactory(get()) }
}

fun httpClientFactory(localSettings: LocalSettings) =
        HttpClient {
            install(HttpTimeout) {
                requestTimeoutMillis = 15_000
            }
            install(ContentNegotiation) {
                json(Json {
                    ignoreUnknownKeys = true
                    prettyPrint = true
                }, contentType = ContentType.Any)

            }
            install(Logging) {
                level = LogLevel.ALL
                logger = object : Logger {
                    override fun log(message: String) {
                        println(message)
                    }
                }
            }
        }.also {
            it.plugin(HttpSend).intercept { request ->
                request.url.host = localSettings.host
                request.url.port = localSettings.port
                execute(request)
            }
        }