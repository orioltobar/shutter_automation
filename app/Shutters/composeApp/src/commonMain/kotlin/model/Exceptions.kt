package model

sealed class Exceptions(open val exception: Throwable): Throwable()
data class RequestException(override val exception: Throwable): Exceptions(exception)
data class ParseException(override val exception: Throwable): Exceptions(exception)
data class ServerException(override val exception: Throwable): Exceptions(exception)
data class NetworkException(override val exception: Throwable): Exceptions(exception)
data class UnknownException(override val exception: Throwable): Exceptions(exception)
