package data

import data.model.AutomationData
import data.model.ErrorData
import data.model.ResponseStatus.Success
import data.model.ResponseWrapper
import data.model.StatusData
import io.ktor.client.HttpClient
import io.ktor.client.call.body
import io.ktor.client.plugins.ClientRequestException
import io.ktor.client.plugins.ResponseException
import io.ktor.client.plugins.ServerResponseException
import io.ktor.client.request.get
import io.ktor.client.request.parameter
import io.ktor.client.request.put
import io.ktor.client.request.url
import io.ktor.serialization.JsonConvertException
import io.ktor.utils.io.errors.IOException
import model.Automation
import model.AutomationType
import model.BlindType
import model.NetworkException
import model.ParseException
import model.RequestException
import model.ServerException
import model.Status
import model.UnknownException


class ShutterServiceImpl(
    private val localSettings: LocalSettings,
    private val httpClient: HttpClient
) : ShuttersApi {
    override fun setHost(host: String) {
        localSettings.host = host
    }

    override fun getHost() = localSettings.host

    override fun getPort() = localSettings.port

    override fun setPort(port: Int) {
        localSettings.port = port
    }

    override suspend fun getState() =
        safeRequest {
            httpClient.get { url("/status") }
                .body<ResponseWrapper<StatusData>>()
                .unwrap()
        }.map { it.toPresentation() }

    override suspend fun triggerDown(blindType: BlindType): Result<Unit> =
        httpClient.get {
            url { url("/down/trigger") }
            parameter(BLIND_PARAM, blindType.value)
        }
            .body<ResponseWrapper<Unit>>()
            .unwrap()

    override suspend fun triggerUp(blindType: BlindType): Result<Unit> =
        httpClient.get {
            url { url("/up/trigger") }
            parameter(BLIND_PARAM, blindType.value)
        }.body<ResponseWrapper<Unit>>()
            .unwrap()

    override suspend fun setAutomationActive(automation: Automation): Result<Automation> {
        return when (automation.type) {
            AutomationType.Rise -> setAlarmUp(isActive = automation.isActive)
            AutomationType.Lower -> setAlarmDown(isActive = automation.isActive)
        }
    }

    override suspend fun setAutomationTime(automation: Automation): Result<Automation> {
        return when (automation.type) {
            AutomationType.Rise -> setAlarmUp(time = automation.time)
            AutomationType.Lower -> setAlarmDown(time = automation.time)
        }
    }

    override suspend fun setAlarmDown(time: String?, isActive: Boolean?): Result<Automation> =
        httpClient.put {
            url { url("/down") }
            isActive?.let { parameter("is_active", isActive) }
            time?.let { parameter("time", it) }
        }.body<ResponseWrapper<AutomationData>>()
            .unwrap()
            .map { it.toPresentation("Lower") }

    override suspend fun setAlarmUp(time: String?, isActive: Boolean?): Result<Automation> =
        httpClient.put {
            url { url("/up") }
            isActive?.let { parameter("is_active", isActive) }
            time?.let { parameter("time", it) }
        }.body<ResponseWrapper<AutomationData>>()
            .unwrap()
            .map { it.toPresentation("Rise") }

    override suspend fun stop(blindType: BlindType) =
        safeRequest {
            httpClient.get {
                url("/stop")
                parameter(BLIND_PARAM, blindType.value)
            }
                .body<ResponseWrapper<Unit>>()
                .unwrap()
        }

    private suspend fun <T> safeRequest(block: suspend () -> Result<T>) =
        try {
            block()
        } catch (exception: ClientRequestException) {
            Result.failure(RequestException(exception))
        } catch (exception: ServerResponseException) {
            Result.failure(ServerException(exception))
        } catch (exception: ResponseException) {
            Result.failure(exception)
        } catch (exception: IOException) {
            Result.failure(NetworkException(exception))
            //SocketException
        } catch (exception: JsonConvertException) {
            Result.failure(ParseException(exception))
        } catch (exception: Exception) {
            Result.failure(UnknownException(exception))
        }

    private fun <T> ResponseWrapper<T>.unwrap() =
        when (status) {
            Success.value -> Result.success(data)
            else -> {
                val message =
                    if (data is ErrorData) {
                        data.title
                    } else {
                        "Something went wrong..."
                    }
                Result.failure(Exception(message))
            }
        }

    private fun StatusData.toPresentation() =
        Status(
            time = time,
            day = day,
            automations = listOf(alarmUp.toPresentation("Rise"), alarmDown.toPresentation("Lower"))
        )

    private fun AutomationData.toPresentation(type: String) =
        Automation(
            time = time,
            isActive = isActive,
            isTriggered = isTriggered ?: false,
            type = AutomationType.valueOf(type),
            id = 0
        )

    private companion object {
        const val BLIND_PARAM = "blind"
    }
}


