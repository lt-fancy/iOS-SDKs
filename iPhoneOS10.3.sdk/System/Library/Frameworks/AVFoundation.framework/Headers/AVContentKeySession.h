/*
    File: AVContentKeySession.h

    Copyright (c) 2015-2016 Apple Inc. All rights reserved.
*/

#import <AVFoundation/AVBase.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/*!
  @class      AVContentKeySession

  @abstract
    An AVContentKeySession is used to create and track decryption keys for media data. Objects conforming to the AVContentKeyRecipient protocol, such as AVURLAssets, can be added to an AVContentKeySession to employ the services of the AVContentKeySession in handling new key requests and to obtain access to the session's already existing keys.

    Its secondary purpose is to provide a report of expired sessions to assist a controlling entity that wishes to track the set of sessions that are still active. If initialized with a location at which to store them, AVContentKeySession maintains a global collection of pending "expired session reports", each associated with an identifier for the app that created the session. The contents of this identifier are specified by the controlling entity that provides media data or that grants permission for its use.
    Expired sessions are tracked as follows: a stream processing session is considered to be started after an instance of AVContentKeySession is created and the first object conforming to the AVContentKeyRecipient protocol is added to it. If an instance of AVContentKeySession that has reached this state does not receive an expire message before it's deallocated or the process in which it's running is terminated, an "expired session report" will subsequently be added to the pending list of expired session reports that indicates that the session expired abnormally. In contrast, for AVContentKeySessions that reach the state of having at least one object conforming to the AVContentKeyRecipient protocol added to them and later receive an expire message, "expired session reports" will be generated that indicate that the session expired normally.
    To obtain the collection of pending expired session reports in order to provide them to the controlling entity associated with a specific app identifier, use +pendingExpiredSessionReportsWithAppIdentifier:.
    After pending expired session reports have been sent to the controlling entity and their receipt has been acknowledged, they can be removed from the collection of pending expired session reports maintained by AVContentKeySession by using +removePendingExpiredSessionReports:withAppIdentifier:.
*/

@class AVContentKeySessionInternal;
@class AVContentKeyRequest;
@class AVContentKeyResponse;
@class AVPersistableContentKeyRequest;
@protocol AVContentKeyRecipient;
@protocol AVContentKeySessionDelegate;

/*!
 @group         AVContentKeySystem string constants
 @brief         Used by AVContentKeySession to determine the method of key delivery
 */
typedef NSString *AVContentKeySystem NS_STRING_ENUM API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

/*!
 @constant      AVContentKeySystemFairPlayStreaming
 @discussion    Used to specify FairPlay Streaming (FPS) as the method of key delivery.
 */
AVF_EXPORT AVContentKeySystem const AVContentKeySystemFairPlayStreaming API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2))
@interface AVContentKeySession : NSObject {
@private
    AVContentKeySessionInternal *_session;
}

/*!
 @method        contentKeySessionWithKeySystem:storageDirectoryAtURL:
 @abstract      Creates a new instance of AVContentKeySession to manage a collection of media content keys.
 @param         keySystem
                A valid key system for retrieving keys.
 @param         storageURL
                Optional URL to a writable directory that the session will use to facilitate expired session reports after abnormal session termination. Pass nil if you do not require expired session reports.
 @result        A new AVContentKeySession.
 @discussion    This method returns an AVContentKeySession instance that is capable of managing collection of media content keys corresponding to the input keySystem. An NSInvalidArgumentException will be raised if the value of keySystem is unsupported.
 */
+ (instancetype)contentKeySessionWithKeySystem:(AVContentKeySystem)keySystem storageDirectoryAtURL:(nullable NSURL *)storageURL;

/*!
 @method        setDelegate:queue:
 @abstract      Sets the receiver's delegate. A delegate is required to handle content key initialization.
 @param         delegate
                An object conforming to the AVContentKeySessionDelegate protocol.
 @param         delegateQueue
                A dispatch queue on which delegate methods will be invoked whenever processes requiring content keys are executed asynchronously. Passing a value of nil for the delegateQueue parameter along with a non-nil value for the delegate parameter will result in an invalid argument exception.
*/
- (void)setDelegate:(nullable id <AVContentKeySessionDelegate>)delegate queue:(nullable dispatch_queue_t)delegateQueue;

/*!
 @property      delegate
 @abstract      The receiver's delegate.
 @discussion    The value of this property is an object conforming to the AVContentKeySessionDelegate protocol. The delegate is set using the setDelegate:queue: method.
*/
@property (readonly, weak, nullable) id<AVContentKeySessionDelegate> delegate;

/*!
 @property      delegateQueue
 @abstract      The dispatch queue on which all delegate methods will be invoked whenever processes requiring content keys are executed asynchronously.
 @discussion    The value of this property is a dispatch_queue_t. The queue is set using the setDelegate:queue: method.
*/
@property (readonly, nullable) dispatch_queue_t delegateQueue;

/*!
 @property      storageURL
 @abstract      The storage URL provided when the AVContentKeySession was created. May be nil.
 @discussion    URL to a writable directory; may be nil. The session will use this to facilitate expired session reports after abnormal session termination.
*/
@property (readonly, nullable) NSURL *storageURL;

/*!
 @property      keySystem
 @abstract      The key system used for retrieving keys
*/
@property (readonly) AVContentKeySystem keySystem;

/*! 
 @method        expire
 @abstract      Tells the receiver to treat the session as having been intentionally and normally expired.
 @discussion    When an instance of AVContentKeySession receives an expire message, all of its associated objects conforming to the AVContentKeyRecipient protocol will become inoperable. Send this message only after you have finished operating on the media data.
*/
- (void)expire;

/*!
 @property      contentProtectionSessionIdentifier
 @abstract      An opaque identifier for the current content protection session. 
 @discussion    May be nil. Will call the delegate's contentKeySessionContentProtectionSessionIdentifierDidChange: when the identifier changes. The protection session ID is a unique string identifier generated by the AVContentKeySession that can be used by the application to identify content key session objects.
*/
@property (readonly, nullable) NSData *contentProtectionSessionIdentifier;

/*!
 @method        processContentKeyRequestWithIdentifier:initializationData:options:
 @abstract      Informs the receiver that it should attempt to instantiate a content decryption key using the specified initialization data.
 @param         identifier
                Container- and protocol-specific identifier to be used to obtain a key response. Either identifier or initializationData must be non-nil. Both can be non-nil, if the content protection protocol requires both.
 @param         initializationData
                Container- and protocol-specific data to be used to obtain a key response. Either identifier or initializationData must be non-nil. Both can be non-nil, if the content protection protocol requires both.
 @param         options
                No options are defined at this time, may be nil.
 @discussion    May be used to generate an AVContentKeyRequest from request initialization data already in hand, without awaiting such data during the processing of media data of an associated recipient.
*/
- (void)processContentKeyRequestWithIdentifier:(nullable id)identifier initializationData:(nullable NSData *)initializationData options:(nullable NSDictionary<NSString *, id> *)options;

/*!
 @method        renewExpiringResponseDataForContentKeyRequest:
 @abstract      Informs the receiver that the already provided response data for an earlier AVContentKeyRequest will imminently expire.
 @discussion    In response the receiver will invoke your delegate with a new content key request entreating it to renew the expiring response data, via -contentKeySession:didProvideRenewingContentKeyRequest:.
*/
- (void)renewExpiringResponseDataForContentKeyRequest:(AVContentKeyRequest *)contentKeyRequest;

@end

@interface AVContentKeySession (AVContentKeyRecipients)

/*! 
 @method        addContentKeyRecipient:
 @abstract      Informs the receiver that the specified recipient will be used for the session.
 @discussion    It is an error to add recipient to sessions that have received an expire message. It is also an error to add recipients after they have already begun to process media data (e.g. after an AVURLAsset has loaded the values of any of its keys). Such errors will result in NSInternalInconsistencyExceptions. Sending this message to an AVContentKeySession is atomic.
*/
- (void)addContentKeyRecipient:(id<AVContentKeyRecipient>)recipient;

/*! 
 @method        removeContentKeyRecipient:
 @abstract      Informs the receiver that the specified recipient will no longer be used.
 @discussion    After the specified recipient is removed from the receiver it will become inoperable. Remove the recipient only after you have finished operating on the media data associated with it. Sending this message to an AVContentKeySession is atomic.
*/
- (void)removeContentKeyRecipient:(id<AVContentKeyRecipient>)recipient;

/*!
 @property      contentKeyRecipients
 @abstract      The array of recipients of content keys currently associated with the AVContentKeySession.
*/
@property (readonly) NSArray<id<AVContentKeyRecipient>> *contentKeyRecipients;

@end

@interface AVContentKeySession (AVContentKeySessionPendingExpiredSessionReports)

/*! 
 @method        pendingExpiredSessionReportsWithAppIdentifier:storageDirectoryAtURL:
 @abstract      Provides "expired session reports" for prior AVContentKeySessions created with the specified app identifier that have expired either normally or abnormally.
 @param         appIdentifier
                An opaque identifier for the application. The contents of this identifier depend on the particular protocol in use by the entity that controls the use of the media data.
 @param         storageURL
                URL to a directory previously used with one or more instances of AVContentKeySession for the storage of expired session reports.
 @result        An NSArray containing instances of NSDictionary, each containing a pending expired session report. These contents depend on the particular protocol in use by the entity that controls the use of the media data.
 @discussion    Note that no reports for sessions still in progress will be included.
*/
+ (NSArray <NSDictionary *> *)pendingExpiredSessionReportsWithAppIdentifier:(NSData *)appIdentifier storageDirectoryAtURL:(NSURL *)storageURL;

/*! 
 @method        removePendingExpiredSessionReports:withAppIdentifier:storageDirectoryAtURL:
 @abstract      Removes expired session reports for prior AVContentKeySessions from storage. Once they have been removed, they will no longer be available via subsequent invocations of +pendingExpiredSessionReportsWithAppIdentifier:.
 @param         expiredSessionReports
                An array of expired session reports to be discarded.
 @param         appIdentifier
                An opaque identifier for the application. The contents of this identifier depend on the particular protocol in use by the entity that controls the use of the media data.
 @param         storageURL
                URL to a writable folder.
 @discussion    This method is most suitable for use only after the specified expired session reports have been sent to the entity that controls the use of the media data and the entity has acknowledged their receipt.
*/
+ (void)removePendingExpiredSessionReports:(NSArray <NSDictionary *> *)expiredSessionReports withAppIdentifier:(NSData *)appIdentifier storageDirectoryAtURL:(NSURL *)storageURL;

@end

/*!
 @group         AVContentKeyRequestRetryReason string constants
 @brief         Used to specify a reason for asking the client to retry a content key request.
 */
typedef NSString *AVContentKeyRequestRetryReason NS_STRING_ENUM API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

/*!
 @constant      AVContentKeyRequestRetryReasonTimedOut
 @discussion    Indicates that the content key request should be retried because the key response was not set soon enough either due the initial request/response was taking too long, or a lease was expiring in the meantime.
 */
AVF_EXPORT AVContentKeyRequestRetryReason const AVContentKeyRequestRetryReasonTimedOut API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

/*!
 @constant      AVContentKeyRequestRetryReasonReceivedResponseWithExpiredLease
 @discussion    Indicates that the content key request should be retried because a key response with expired lease was set on the previous content key request.
 */
AVF_EXPORT AVContentKeyRequestRetryReason const AVContentKeyRequestRetryReasonReceivedResponseWithExpiredLease API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

/*!
 @constant      AVContentKeyRequestRetryReasonReceivedObsoleteContentKey
 @discussion    Indicates that the content key request should be retried because an obsolete key response was set on the previous content key request.
 */
AVF_EXPORT AVContentKeyRequestRetryReason const AVContentKeyRequestRetryReasonReceivedObsoleteContentKey API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2))
@protocol AVContentKeySessionDelegate <NSObject>

/*!
 @method        contentKeySession:didProvideContentKeyRequest:
 @abstract      Provides the receiver with a new content key request.
 @param         session
                An instance of AVContentKeySession that's providing a new content key request.
 @param         keyRequest
                An instance of AVContentKeyRequest.
 @discussion    Will be invoked by an AVContentKeySession as a result of a call to -processContentKeyRequestWithIdentifier:initializationData:options:. Can be invoked during playback or other processing of an AVURLAsset added to the AVContentKeySession.
*/
@required
- (void)contentKeySession:(AVContentKeySession *)session didProvideContentKeyRequest:(AVContentKeyRequest *)keyRequest;

/*!
 @method        contentKeySession:didProvideRenewingContentKeyRequest:
 @abstract      Provides the receiver with a new content key request representing a renewal of an existing content key.
 @param         session
                An instance of AVContentKeySession that's providing a new content key request.
 @param         keyRequest
                An instance of AVContentKeyRequest representing renewal of a previous content key.
 @discussion    Will be invoked by an AVContentKeySession as the result of a call to -renewExpiringResponseDataForContentKeyRequest:.
*/
@optional
- (void)contentKeySession:(AVContentKeySession *)session didProvideRenewingContentKeyRequest:(AVContentKeyRequest *)keyRequest;

/*!
 @method        contentKeySession:didProvidePersistableContentKeyRequest:
 @abstract      Provides the receiver with a new content key request that allows key persistence.
 @param         session
                An instance of AVContentKeySession that's providing a new content key request.
 @param         keyRequest
                An instance of AVPersistableContentKeyRequest.
 @discussion    Will be invoked by an AVContentKeyRequest as the result of a call to -respondByRequestingPersistableContentKeyRequest.
 */
@optional
- (void)contentKeySession:(AVContentKeySession *)session didProvidePersistableContentKeyRequest:(AVPersistableContentKeyRequest *)keyRequest;

/*!
 @method        contentKeySession:contentKeyRequest:didFailWithError:
 @abstract      Informs the receiver a content key request has failed.
 @param         session
                The instance of AVContentKeySession that initiated the content key request.
 @param         keyRequest
                The instance of AVContentKeyRequest that has failed.
 @param         error
                An instance of NSError that describes the failure that occurred.
*/
@optional
- (void)contentKeySession:(AVContentKeySession *)session contentKeyRequest:(AVContentKeyRequest *)keyRequest didFailWithError:(NSError *)err;

/*!
 @method        contentKeySession:shouldRetryContentKeyRequest:reason:
 @abstract      Provides the receiver a content key request that should be retried because a previous content key request failed.
 @param         session
                An instance of AVContentKeySession that's providing the content key request that should be retried.
 @param         keyRequest
                An instance of AVContentKeyRequest that should be retried.
 @param         retryReason
                An enum value to explain why the receiver could retry the new content key request.
 @result        A BOOL value indicating receiver's desire to retry the failed content key request.
 @discussion    Will be invoked by an AVContentKeySession when a content key request should be retried. The reason for failure of previous content key request is specified. The receiver can decide if it wants to request AVContentKeySession to retry this key request based on the reason. If the receiver returns YES, AVContentKeySession would restart the key request process. If the receiver returns NO or if it does not implement this delegate method, the content key request would fail and AVContentKeySession would let the receiver know through -contentKeySession:contentKeyRequest:didFailWithError:.
 */
@optional
- (BOOL)contentKeySession:(AVContentKeySession *)session shouldRetryContentKeyRequest:(AVContentKeyRequest *)keyRequest reason:(AVContentKeyRequestRetryReason)retryReason;

/*!
 @method        contentKeySessionContentProtectionSessionIdentifierDidChange:
 @abstract      Informs the receiver that the value of -[AVContentKeySession contentProtectionSessionIdentifier] has changed.
*/
@optional
- (void)contentKeySessionContentProtectionSessionIdentifierDidChange:(AVContentKeySession *)session;

@end


/*
 @constant    AVContentKeyRequestStatusRequestingResponse
    Indicates that the request has just been created.
 @constant    AVContentKeyRequestStatusReceivedResponse
    Indicates that a response to a key reequest was received and key is in use. This does not indicate that the key is valid.
 @constant    AVContentKeyRequestStatusRenewed
    Indicates that the key request was renewed. This does not indicate that the key is valid.
 @constant    AVContentKeyRequestStatusRetried
	Indicates that the key request was retried.
 @constant    AVContentKeyRequestStatusCancelled
    Indicates that the key request was cancelled.
 @constant    AVContentKeyRequestStatusFailed
    Indicates that the request has encountered an error. See also the error property.
*/
typedef NS_ENUM(NSInteger, AVContentKeyRequestStatus) {
    AVContentKeyRequestStatusRequestingResponse,
    AVContentKeyRequestStatusReceivedResponse,
    AVContentKeyRequestStatusRenewed,
	AVContentKeyRequestStatusRetried,
    AVContentKeyRequestStatusCancelled,
    AVContentKeyRequestStatusFailed
} API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

@class AVContentKeyRequestInternal;

API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2))
@interface AVContentKeyRequest : NSObject
{
@private
    AVContentKeyRequestInternal *_contentKeyRequest;
}

/*
 @property      status
 @abstract      This describes the state of the AVContentKeyRequest, value is one of AVContentKeyRequestStatus.
*/
@property (readonly) AVContentKeyRequestStatus status;

/*!
 @property      error
 @abstract      If the receiver's status is AVContentKeyRequestStatusFailed, this describes the error that caused the failure.
 @discussion    The value of this property is an NSError that describes what caused the content key request to fail. If the receiver's status is not AVContentKeyRequestStatusFailed, the value of this property is nil.
 */
@property (readonly, nullable) NSError *error;

/*
 @property      identifier
 @abstract      Container- and protocol-specific identifier for the content key.
 @discussion    In order to use a key with an HTTP Live Streaming AVURLAsset, the identifier must be an NSURL that matches a key URI in the Media Playlist.
*/
@property (readonly, nullable) id identifier;

/*
 @property      initializationData
 @abstract      Container- and protocol-specific data to be used to obtain a key response.
*/
@property (nonatomic, readonly, nullable) NSData *initializationData;

/*
 @property      canProvidePersistableContentKey
 @abstract      When the value of this property is YES, you can use the method -persistableContentKeyFromKeyVendorResponse:options:error: to create a persistable content key from the content key response.
 @dicsussion    The value of this property will be YES only when the receiver is provided to your AVContentKeySession delegate via the method -contentKeySession:didProvidePersistableContentKeyRequest:. If you have an AVContentKeyRequest for which the value of canProvidePersistableContentKey is NO, but you wish to obtain a persistable content key, send the AVContentKeyRequest the message -respondByRequestingPersistableContentKeyRequest.
 */
@property (readonly) BOOL canProvidePersistableContentKey;

/*!
 @method        makeStreamingContentKeyRequestDataForApp:contentIdentifier:options:completionHandler:
 @abstract      Obtains a content key request data for a specific combination of application and content.
 @param         appIdentifier
                An opaque identifier for the application. The value of this identifier depends on the particular system used to provide the content key.
 @param         contentIdentifier
                An opaque identifier for the content. The value of this identifier depends on the particular system used to provide the content key. May be nil.
 @param         options
                Additional information necessary to obtain the key, or nil if none. See AVContentKeyRequest*Key below.
 @param         handler
                Once the streaming content key request is prepared, this block will be called with the request data or an error describing the failure.
 @discussion    If option AVContentKeyRequestProtocolVersionsKey is not specified the default protocol version of 1 is assumed.
*/
- (void)makeStreamingContentKeyRequestDataForApp:(NSData *)appIdentifier
                               contentIdentifier:(nullable NSData *)contentIdentifier
                                         options:(nullable NSDictionary<NSString *, id> *)options
                               completionHandler:(void (^)(NSData * _Nullable contentKeyRequestData, NSError * _Nullable error))handler;

/*!
 @method        processContentKeyResponse:
 @abstract      Informs the receiver to process the specified content key response.
 @param         keyResponse
                An instance of AVContentKeyResponse carrying a response to a content key request.
 @discussion    After you receive an AVContentKeyRequest via -contentKeySession:didProvideContentKeyRequest: and after you invoke -[AVContentKeyRequest makeStreamingContentKeyRequestDataForApp:contentIdentifier:options:completionHandler:] on that request, you must obtain a response to the request in accordance with the protocol in use by the entity that controls the use of the media data. This is the method you use to provide the content key response to make protected content available for processing. If obtaining the content key response fails, use -processContentKeyResponseError:.
*/
- (void)processContentKeyResponse:(AVContentKeyResponse *)keyResponse;

/*!
 @method        processContentKeyResponseError:
 @abstract      Informs the receiver that obtaining a content key response has failed, resulting in failure handling.
 @param         error
                An instance of NSError that describes the specific failure that occurred.
*/
- (void)processContentKeyResponseError:(NSError *)error;

/*!
 @method        respondByRequestingPersistableContentKeyRequest
 @abstract      Informs the receiver to process a persistable content key request.
 @discussion    When you receive an AVContentKeyRequest via -contentKeySession:didProvideContentKeyRequest: and you want the resulting key response to produce a key that can persist across multiple playback sessions, you must invoke -respondByRequestingPersistableContentKeyRequest on that AVContentKeyRequest in order to signal that you want to process an AVPersistableContentKeyRequest instead. If the underlying protocol supports persistable content keys, in response your delegate will receive an AVPersistableContentKeyRequest via -contentKeySession:didProvidePersistableContentKeyRequest:.
 */
- (void)respondByRequestingPersistableContentKeyRequest;

@end

API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2))
@interface AVPersistableContentKeyRequest : AVContentKeyRequest

/*!
 @method        persistableContentKeyFromKeyVendorResponse:options:error:
 @abstract      Obtains a persistable content key from a context.
 @param         keyVendorResponse
                The response returned from the key vendor as a result of a request generated from makeStreamingContentKeyRequestDataForApp:contentIdentifier:options:completionHandler:.
 @param         options
                Additional information necessary to obtain the persistable content key, or nil if none.
 @param         error
                If obtaining the persistable content key fails, will be set to an instance of NSError describing the failure.
 @result        The persistable content key data that may be stored offline to answer future loading requests of the same content key.
 @discussion    The data returned from this method may be used to immediately satisfy an AVPersistableContentKeyRequest, as well as any subsequent requests for the same key url using processContentKeyResponse: method. When you receive an AVContentKeyRequest via -contentKeySession:didProvideContentKeyRequest: and you want to use existing persistent content key from storage, you must invoke -respondByRequestingPersistableContentKeyRequest on that AVContentKeyRequest in order to signal that you want to process an AVPersistableContentKeyRequest instead. If the underlying protocol supports persistable content keys, in response your delegate will receive an AVPersistableContentKeyRequest via -contentKeySession:didProvidePersistableContentKeyRequest:. You can set the persistent key from storage on the AVPersistableContentKeyRequest using processContentKeyResponse:.
 */
- (NSData *)persistableContentKeyFromKeyVendorResponse:(NSData *)keyVendorResponse
                                              options:(nullable NSDictionary <NSString *, id> *)options
                                                error:(NSError **)outError;

@end

@interface AVContentKeyRequest (AVContentKeyRequestRenewal)

/*
 @property      renewsExpiringResponseData
 @abstract      Indicates whether the receiver represents a request to renew previously provided response data that is expiring or has expired.
*/
@property (readonly) BOOL renewsExpiringResponseData;

@end

@class AVContentKeyResponseInternal;

/*!
 @class         AVContentKeyResponse
 @abstract      AVContentKeyResponse is used to represent the data returned from the key server when requesting a key for decrypting content.
*/
API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2))
@interface AVContentKeyResponse : NSObject
{
@private
    AVContentKeyResponseInternal *_keyResponse;
}

/*!
 @method        contentKeyResponseWithFairPlayStreamingKeyResponseData:
 @abstract      Create an AVContentKeyResponse from the server response to a key request made when using FairPlayStreaming (FPS) as the method of key delivery.
 @param         keyResponseData
                The response from the FairPlayStreaming key server
 @result        A new AVContentKeyResponse holding data from a FairPlayStreaming key server that is used to decrypt the content
 @discussion    The object created by this method is typically used with an AVContentKeyRequest created by an AVContentKeySession using keySystem AVContentKeySystemFairPlayStreaming. It is passed to AVContentKeyRequest -processContentKeyResponse: in order to supply the decryptor with key data
*/
+ (instancetype)contentKeyResponseWithFairPlayStreamingKeyResponseData:(NSData *)keyResponseData;

@end

// Options keys for use with -[AVContentKeySession makeStreamingContentKeyRequestDataForApp:contentIdentifier:options:completionHandler:]
/*!
 @constant      AVContentKeyRequestProtocolVersionsKey
 @abstract      Specifies the versions of the content protection protocol supported by the application as an NSArray of one or more NSNumber objects.
 */
AVF_EXPORT NSString *const AVContentKeyRequestProtocolVersionsKey API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2));

/*!
  @protocol      AVContentKeyRecipient

  @abstract
    Classes of objects that may require decryption keys for media data in order to enable processing, such as parsing or playback, conform to this protocol.
*/
API_AVAILABLE(macosx(10.12.4), ios(10.3), tvos(10.2))
@protocol AVContentKeyRecipient

@required

/*!
 @property      mayRequireContentKeysForMediaDataProcessing
 @abstract      Indicates whether the receiver may require decryption keys for media data in order to enable processing.
 @discussion    When the value of mayRequireContentKeysForMediaDataProcessing is YES, adding the receiver to an AVContentKeySession allows it to employ the session's already existing keys and also enables the handling of new key requests by the AVContentKeySession's delegate.
*/
@property (nonatomic, readonly) BOOL mayRequireContentKeysForMediaDataProcessing;

@end

NS_ASSUME_NONNULL_END
